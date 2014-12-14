/*
 * Copyright (C) ????-2004 Justin Hibbits <jrh29@po.cwru.edu>
 * Copyright (C) 2004 Thomer M. Gil <mutt@thomer.com>
 * 
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 */ 


#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "mutt.h"
#include "mutt_menu.h"
#include "mutt_curses.h"
#include "sidebar.h"
#include "buffy.h"
#include <libgen.h>
#include "keymap.h"
#include <stdbool.h>
#include <wchar.h>

/*BUFFY *CurBuffy = 0;*/
static BUFFY *TopBuffy = 0;
static BUFFY *BottomBuffy = 0;
static int known_lines = 0;

static int quick_log10(int n)
{
        char string[32];
        sprintf(string, "%d", n);
        return strlen(string);
}

void calc_boundaries (int menu)
{
	BUFFY *tmp = Incoming;

	if ( known_lines != LINES ) {
		TopBuffy = BottomBuffy = 0;
		known_lines = LINES;
	}
	for ( ; tmp->next != 0; tmp = tmp->next )
		tmp->next->prev = tmp;

	if ( TopBuffy == 0 && BottomBuffy == 0 )
		TopBuffy = Incoming;
	if ( BottomBuffy == 0 ) {
		int count = LINES - 2 - (menu != MENU_PAGER || option(OPTSTATUSONTOP));
		BottomBuffy = TopBuffy;
		while ( --count && BottomBuffy->next )
			BottomBuffy = BottomBuffy->next;
	}
	else if ( TopBuffy == CurBuffy->next ) {
		int count = LINES - 2 - (menu != MENU_PAGER);
		BottomBuffy = CurBuffy;
		tmp = BottomBuffy;
		while ( --count && tmp->prev)
			tmp = tmp->prev;
		TopBuffy = tmp;
	}
	else if ( BottomBuffy == CurBuffy->prev ) {
		int count = LINES - 2 - (menu != MENU_PAGER);
		TopBuffy = CurBuffy;
		tmp = TopBuffy;
		while ( --count && tmp->next )
			tmp = tmp->next;
		BottomBuffy = tmp;
	}
}

char *make_sidebar_entry(char *box, int size, int new, int flagged)
{
    char int_store[20]; // up to 64 bits integers
    int right_width, left_width;
    int box_len, box_bytes;
    int int_len;
    int right_offset = 0;
    int delim_len = mbstowcs(NULL, NONULL(SidebarDelim), 0);
    static char *entry;

    right_width = left_width = 0;
    box_len = box_bytes = 0;

    // allocate an entry big enough to contain SidebarWidth wide chars
    entry = malloc((SidebarWidth*4)+1); // TODO: error check

    // determine the right space (i.e.: how big are the numbers that we want to print)
    if ( size > 0 ) {
        int_len = snprintf(int_store, sizeof(int_store), "%d", size);
        right_width += int_len;
    } else {
        right_width = 1; // to represent 0
    }
    if ( new > 0 ) {
        int_len = snprintf(int_store, sizeof(int_store), "%d", new);
        right_width += int_len + 2; // 2 is for ()
    }
    if ( flagged > 0 ) {
        int_len = snprintf(int_store, sizeof(int_store), "%d", flagged);
        right_width += int_len + 2; // 2 is for []
    }

    // determine how much space we have for *box and its padding (if any)
    left_width = SidebarWidth - right_width - 1 - delim_len; // 1 is for the space
    //fprintf(stdout, "left_width: %d right_width: %d\n", left_width, right_width);
    // right side overflow case
    if ( left_width <= 0 ) {
        snprintf(entry, SidebarWidth*4, "%-*.*s ...", SidebarWidth-4-delim_len, SidebarWidth-4-delim_len, box);
        return entry;
    }
    right_width -= delim_len;

    // to support utf-8 chars we need to add enough space padding in case there
    // are less chars than bytes in *box
    box_len = mbstowcs(NULL, box, 0);
    box_bytes = strlen(box);
    // debug
    //fprintf(stdout, "box_len: %d box_bytes: %d (diff: %d)\n", box_len, box_bytes, (box_bytes-box_len));
    // if there is less string than the space we allow, then we will add the
    // spaces
    if ( box_len != -1 && box_len < left_width ) {
        left_width += (box_bytes - box_len);
    }
    // otherwise sprintf will truncate the string for us (therefore, no else case)

    // print the sidebar entry (without new and flagged messages, at the moment)
    //fprintf(stdout, "left_width: %d right_width: %d\n", left_width, right_width);
    right_offset = snprintf(entry, SidebarWidth*4, "%-*.*s %d", left_width, left_width, box, size);

    // then pad new and flagged messages if any
    if ( new > 0 ) {
        right_offset += snprintf(entry+right_offset, SidebarWidth*4-right_offset, "(%d)", new);
    }
    if ( flagged > 0 ) {
        right_offset += snprintf(entry+right_offset, SidebarWidth*4-right_offset, "[%d]", flagged);
    }

    return entry;
}

void set_curbuffy(char buf[LONG_STRING])
{
  BUFFY* tmp = CurBuffy = Incoming;

  if (!Incoming)
    return;

  while(1) {
    if(!strcmp(tmp->path, buf)) {
      CurBuffy = tmp;
      break;
    }

    if(tmp->next)
      tmp = tmp->next;
    else
      break;
  }
}

int draw_sidebar(int menu) {

	int lines = option(OPTHELP) ? 1 : 0;
	BUFFY *tmp;
#ifndef USE_SLANG_CURSES
        attr_t attrs;
#endif
        short delim_len = mbstowcs(NULL, NONULL(SidebarDelim), 0);
        short color_pair;

        static bool initialized = false;
        static int prev_show_value;
        static short saveSidebarWidth;

        /* initialize first time */
        if(!initialized) {
                prev_show_value = option(OPTSIDEBAR);
                saveSidebarWidth = SidebarWidth;
                if(!option(OPTSIDEBAR)) SidebarWidth = 0;
                initialized = true;
        }

        /* save or restore the value SidebarWidth */
        if(prev_show_value != option(OPTSIDEBAR)) {
                if(prev_show_value && !option(OPTSIDEBAR)) {
                        saveSidebarWidth = SidebarWidth;
                        SidebarWidth = 0;
                } else if(!prev_show_value && option(OPTSIDEBAR)) {
                        SidebarWidth = saveSidebarWidth;
                }
                prev_show_value = option(OPTSIDEBAR);
        }


//	if ( SidebarWidth == 0 ) return 0;
       if (SidebarWidth > 0 && option (OPTSIDEBAR)
           && delim_len >= SidebarWidth) {
         unset_option (OPTSIDEBAR);
         /* saveSidebarWidth = SidebarWidth; */
         if (saveSidebarWidth > delim_len) {
           SidebarWidth = saveSidebarWidth;
           mutt_error (_("Value for sidebar_delim is too long. Disabling sidebar."));
           sleep (2);
         } else {
           SidebarWidth = 0;
           mutt_error (_("Value for sidebar_delim is too long. Disabling sidebar. Please set your sidebar_width to a sane value."));
           sleep (4); /* the advise to set a sane value should be seen long enough */
         }
         saveSidebarWidth = 0;
         return (0);
       }

    if ( SidebarWidth == 0 || !option(OPTSIDEBAR)) {
      if (SidebarWidth > 0) {
        saveSidebarWidth = SidebarWidth;
        SidebarWidth = 0;
      }
      unset_option(OPTSIDEBAR);
      return 0;
    }

        /* get attributes for divider */
	SETCOLOR(MT_COLOR_STATUS);
#ifndef USE_SLANG_CURSES
        attr_get(&attrs, &color_pair, 0);
#else
        color_pair = attr_get();
#endif
	SETCOLOR(MT_COLOR_NORMAL);

	/* draw the divider */

	for ( ; lines < LINES-1-(menu != MENU_PAGER || option(OPTSTATUSONTOP)); lines++ ) {
		move(lines, SidebarWidth - delim_len);
		addstr(NONULL(SidebarDelim));
#ifndef USE_SLANG_CURSES
                mvchgat(lines, SidebarWidth - delim_len, delim_len, 0, color_pair, NULL);
#endif
	}

	if ( Incoming == 0 ) return 0;
	lines = option(OPTHELP) ? 1 : 0; /* go back to the top */

	if ( known_lines != LINES || TopBuffy == 0 || BottomBuffy == 0 ) 
		calc_boundaries(menu);
	if ( CurBuffy == 0 ) CurBuffy = Incoming;

	tmp = TopBuffy;

	SETCOLOR(MT_COLOR_NORMAL);

	for ( ; tmp && lines < LINES-1 - (menu != MENU_PAGER || option(OPTSTATUSONTOP)); tmp = tmp->next ) {
		if ( tmp == CurBuffy )
			SETCOLOR(MT_COLOR_INDICATOR);
		else if ( tmp->msg_unread > 0 )
			SETCOLOR(MT_COLOR_NEW);
		else if ( tmp->msg_flagged > 0 )
		        SETCOLOR(MT_COLOR_FLAGGED);
		else {
                  /* make sure the path is either:
                     1.  Containing new mail.
                     2.  The inbox.
                     3.  The current box.
                   */
                  if ((option (OPTSIDEBARNEWMAILONLY)) &&
                      ( (tmp->msg_unread <= 0)  &&
                        ( tmp != Incoming ) &&
                        Context &&
                        ( strcmp( tmp->path, Context->path ) != 0 ) ) )
                    continue;
                  else
                    SETCOLOR(MT_COLOR_NORMAL);
                }

		move( lines, 0 );
		if ( Context && !strcmp( tmp->path, Context->path ) ) {
			tmp->msg_unread = Context->unread;
			tmp->msgcount = Context->msgcount;
			tmp->msg_flagged = Context->flagged;
		}
		// check whether Maildir is a prefix of the current folder's path
		short maildir_is_prefix = 0;
		if ( (strlen(tmp->path) > strlen(Maildir)) &&
			(strncmp(Maildir, tmp->path, strlen(Maildir)) == 0) )
        		maildir_is_prefix = 1;
		// calculate depth of current folder and generate its display name with indented spaces
		int sidebar_folder_depth = 0;
		char *sidebar_folder_name;
		int i;
		sidebar_folder_name = tmp->path;
		/* disregard a trailing separator, so strlen() - 2
		 * https://bugs.gentoo.org/show_bug.cgi?id=373197#c16 */
		for (i = strlen(sidebar_folder_name) - 2; i >= 0; i--) {
			if (SidebarDelimChars &&
					strchr(SidebarDelimChars, sidebar_folder_name[i]))
			{
				sidebar_folder_name += i + 1;
				break;
			}
		}
		if ( maildir_is_prefix ) {
			char *tmp_folder_name;
			int lastsep = 0;
			tmp_folder_name = tmp->path + strlen(Maildir);
			for (i = 0; i < strlen(tmp_folder_name) - 1; i++) {
				if (SidebarDelimChars &&
						strchr(SidebarDelimChars, tmp_folder_name[i]))
				{
					sidebar_folder_depth++;
					lastsep = i + 1;
				}
			}
			if (sidebar_folder_depth > 0) {
				tmp_folder_name += lastsep;  /* basename */
				sidebar_folder_name = malloc(strlen(tmp_folder_name) + sidebar_folder_depth + 1);
				for (i=0; i < sidebar_folder_depth; i++)
					sidebar_folder_name[i]=' ';
				sidebar_folder_name[i]=0;
				strncat(sidebar_folder_name, tmp_folder_name, strlen(tmp_folder_name) + sidebar_folder_depth);
			}
		}
		printw( "%.*s", SidebarWidth - delim_len + 1,
			make_sidebar_entry(sidebar_folder_name, tmp->msgcount,
			tmp->msg_unread, tmp->msg_flagged));
		if (sidebar_folder_depth > 0)
		        free(sidebar_folder_name);
		lines++;
	}
	SETCOLOR(MT_COLOR_NORMAL);
	for ( ; lines < LINES-1 - (menu != MENU_PAGER || option(OPTSTATUSONTOP)); lines++ ) {
		int i = 0;
		move( lines, 0 );
		for ( ; i < SidebarWidth - delim_len; i++ )
			addch(' ');
	}
	return 0;
}

BUFFY * exist_next_new()
{
       BUFFY *tmp = CurBuffy;
       if(tmp == NULL) return NULL;
       while (tmp->next != NULL)
       {
              tmp = tmp->next;
               if(tmp->msg_unread) return tmp;
       }
       return NULL;
}

BUFFY * exist_prev_new()
{
       BUFFY *tmp = CurBuffy;
       if(tmp == NULL) return NULL;
       while (tmp->prev != NULL)
       {
               tmp = tmp->prev;
               if(tmp->msg_unread) return tmp;
       }
       return NULL;
}

void set_buffystats(CONTEXT* Context)
{
        BUFFY *tmp = Incoming;
        while(tmp) {
                if(Context && !strcmp(tmp->path, Context->path)) {
			tmp->msg_unread = Context->unread;
			tmp->msgcount = Context->msgcount;
                        break;
                }
                tmp = tmp->next;
        }
}

void scroll_sidebar(int op, int menu)
{
        BUFFY *tmp;
        if(!SidebarWidth) return;
        if(!CurBuffy) return;

	switch (op) {
		case OP_SIDEBAR_NEXT:
                if (!option (OPTSIDEBARNEWMAILONLY)) {
			if ( CurBuffy->next == NULL ) return;
			CurBuffy = CurBuffy->next;
			break;
                }
                case OP_SIDEBAR_NEXT_NEW:
                        if ( (tmp = exist_next_new()) == NULL)
                                return;
                        else CurBuffy = tmp;
                        break;
		case OP_SIDEBAR_PREV:
                 if (!option (OPTSIDEBARNEWMAILONLY)) {
			if ( CurBuffy->prev == NULL ) return;
			CurBuffy = CurBuffy->prev;
			break;
                }
                case OP_SIDEBAR_PREV_NEW:
                       if ( (tmp = exist_prev_new()) == NULL)
                               return;
                       else CurBuffy = tmp;
                       break;
		case OP_SIDEBAR_SCROLL_UP:
			CurBuffy = TopBuffy;
			if ( CurBuffy != Incoming ) {
				calc_boundaries(menu);
				CurBuffy = CurBuffy->prev;
			}
			break;
		case OP_SIDEBAR_SCROLL_DOWN:
			CurBuffy = BottomBuffy;
			if ( CurBuffy->next ) {
				calc_boundaries(menu);
				CurBuffy = CurBuffy->next;
			}
			break;
                case OP_SIDEBAR_NEW:
                	if ( (tmp = exist_next_new()) == NULL)
				tmp = TopBuffy;
			if ( tmp->msg_unread == 0 ) {
				CurBuffy = tmp;
				tmp = exist_next_new();
			}
			if ( tmp != NULL )
				CurBuffy = tmp;
			break;
		default:
			return;
	}
	calc_boundaries(menu);
	draw_sidebar(menu);
}

