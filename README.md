mutt-extra patched with fur
===================================
This mutt repository is 1.5.23 pre-patched with extra fur. This is a clone of archlinux's mutt-patched package.

##### List of pactches

All the applied patches are in mutt-patched directory:
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/228671-pipe-mime.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/383769-score-match.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/467432-write_bcc.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/528233-readonly-open.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/531430-imapuser.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/542817-smimekeys-tmpdir.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/543467-thread-segfault.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/547980-smime_keys-chaining.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/548577-gpgme-1.2.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/553321-ansi-escape-segfault.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/566076-build_doc_adjustments.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/603288-split-fetches.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/611410-no-implicit_autoview-for-text-html.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/Md.etc_mailname_gethostbyname.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/Muttrc.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/PKGBUILD
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/am-maintainer-mode.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/assumed_charset-compat.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/compressed-folders.debian.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/compressed-folders.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/correct_docdir_in_man_page.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/define-pgp_getkeys_command.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/document_debian_defaults.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/dont_document_not_present_features.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/fix-configure-test-operator.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/gpg.rc-paths.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/ifdef.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/imap_fast_trash.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/install
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/multiple-fcc.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/nntp.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/patch-1.5.4.vk.pgp_verbose_mime.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/path_max.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/purge-message.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/sensible_browser_position.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/sidebar-compose.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/sidebar-delimnullwide.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/sidebar-dotpathsep.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/sidebar-new.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/sidebar-newonly.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/sidebar-utf8.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/sidebar.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/smime.rc.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/trash-folder.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/update_german_translation.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/use_usr_bin_editor.patch
    /Volumes/home/epiclulz/Documents/workspace/github/mutt-extra/src/mutt-patched/xtitles.patch

##### Build instructions

  ./configure \
    --prefix=/usr \
    --sysconfdir=/etc \
    --mandir=/usr/share/man \
    --with-docdir=/usr/share/doc \
    --with-mailpath=/var/mail \
    --disable-dependency-tracking \
    --enable-compressed \
    --enable-debug \
    --enable-fcntl \
    --enable-gpgme \
    --enable-hcache \
    --enable-imap \
    --enable-pop \
    --enable-smtp \
    --with-curses \
    --with-gss \
    --with-idn \
    --with-mixmaster \
    --without-bdb \
    --without-gdbm \
    --without-qdbm \
    --with-regex \
    --with-sasl \
    --with-gnutls
    make
    make install

##### Links

    Mutt URL: https://bitbucket.org/mutt/mutt/downloads/mutt-1.5.23.tar.gz
    Archs Mutt Patches: $MUTT_PATCH_URL)

##### Halp?!
God helps those who help themeselves..
