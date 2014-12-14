
### mutt-patched with exta fur
===============================
This mutt repository is 1.5.23 pre-patched with extra fur. This is a clone of archlinux's mutt-patched package.

##### List of pactches
-------------------------------
All the applied patches are in *mutt-patched* directory:

```
  228671-pipe-mime.patch
  383769-score-match.patch
  467432-write_bcc.patch
  528233-readonly-open.patch
  531430-imapuser.patch
  542817-smimekeys-tmpdir.patch
  543467-thread-segfault.patch
  547980-smime_keys-chaining.patch
  548577-gpgme-1.2.patch
  553321-ansi-escape-segfault.patch
  566076-build_doc_adjustments.patch
  603288-split-fetches.patch
  611410-no-implicit_autoview-for-text-html.patch
  Md.etc_mailname_gethostbyname.patch
  Muttrc.patch
  PKGBUILD
  am-maintainer-mode.patch
  assumed_charset-compat.patch
  compressed-folders.debian.patch
  compressed-folders.patch
  correct_docdir_in_man_page.patch
  define-pgp_getkeys_command.patch
  document_debian_defaults.patch
  dont_document_not_present_features.patch
  fix-configure-test-operator.patch
  gpg.rc-paths.patch
  ifdef.patch
  imap_fast_trash.patch
  install
  multiple-fcc.patch
  nntp.patch
  patch-1.5.4.vk.pgp_verbose_mime.patch
  path_max.patch
  purge-message.patch
  sensible_browser_position.patch
  sidebar-compose.patch
  sidebar-delimnullwide.patch
  sidebar-dotpathsep.patch
  sidebar-new.patch
  sidebar-newonly.patch
  sidebar-utf8.patch
  sidebar.patch
  smime.rc.patch
  trash-folder.patch
  update_german_translation.patch
  use_usr_bin_editor.patch
  xtitles.patch
```

##### General mutt build instructions
-------------------------------

 * git clone the directory
 * ./configure && make && make install
 * profit!

###### Example mutt configure options:

```shell
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
```

##### Hax!?
-------------------------------
  * Download [mutt-add-fur.sh](https://raw.githubusercontent.com/hihellobolke/mutt-extra/master/mutt-add-fur.sh)
  * Run it from empty directory

```

cd $(mktemp -d /var/folders/rv/cp3ydg4s42d3850z5y4ys5cm0000gp/T//mutt-extra.XXXXX) && bash <(curl -sk https://raw.githubusercontent.com/hihellobolke/mutt-extra/master/mutt-add-fur.sh)

```

##### Links
-------------------------------

 - [Mutt Src](https://bitbucket.org/mutt/mutt/downloads/mutt-1.5.23.tar.gz)
 - [Arch's mutt patches](https://aur.archlinux.org/packages/mu/mutt-patched/mutt-patched.tar.gz)

##### Halp?!
-------------------------------
God help's those who help themeselves...

