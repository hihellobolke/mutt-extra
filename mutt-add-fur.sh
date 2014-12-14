#!/bin/bash
## -- code --
ESC_SEQ="\x1b["
COL_RESET=$ESC_SEQ"39;49;00m"
COL_RED=$ESC_SEQ"31;01m"
COL_GREEN=$ESC_SEQ"32;01m"
COL_YELLOW=$ESC_SEQ"33;01m"

red(){ echo -e "${COL_RED}$@${COL_RESET}";}
green(){ echo -e "${COL_GREEN}$@${COL_RESET}";}
yellow(){ echo -e "${COL_YELLOW}$@${COL_RESET}";}

sayerror(){ red "\nError: $@" 1>&2;}
saywarn(){ yellow "\nWarn: $@";}
sayinfo(){ green "\nInfo: $@";}
sayline(){ yellow "\n-------------------------------------------------------------\n";}


read -p "Downloading mutt & extracting in current directory ($(pwd)) Continue? [Y/N]: " -n 1 -r
echo

if [[ ! $REPLY =~ ^[Yy]$ ]]
then
    sayerror "Please change to prefered directory and rerun this script..."
    exit 1
else
    sayinfo "Using $(pwd) for downloading & patching mutt.... "
fi

set -e

MYTMP=$(mktemp -d ${TMPDIR:-/tmp}/mutt-extra.XXXXX)
trap 'rm -rf $MYTMPDIR' EXIT

WGET=$(which wget)

MUTT_PKGBUILD_URL=https://aur.archlinux.org/packages/mu/mutt-patched/PKGBUILD
MUTT_VER=$(wget -qO- $MUTT_PKGBUILD_URL | sed -n '/pkgver=/s/.*=//pg')
MUTT_URL=https://bitbucket.org/mutt/mutt/downloads/mutt-${MUTT_VER}.tar.gz
MUTT_SRCDIR=$(pwd)
MUTT_PATCH_URL=https://aur.archlinux.org/packages/mu/mutt-patched/mutt-patched.tar.gz
MUTT_PATCH_DIR=${MUTT_SRCDIR}/$(basename $MUTT_PATCH_URL | sed 's/\.[a-z].*//g')
MUTT_PKGBUILD=${MYTMP}/$(basename $MUTT_PKGBUILD_URL)


sayinfo "Using tmpdir at $MYTMP"
ls -ld $MYTMP

sayinfo "Downloading mutt..."
(cd $MYTMP && $WGET $MUTT_URL)

sayinfo "Extracting ..."
tar -zxvf $MYTMP/$(basename $MUTT_URL) && (cd $(basename $MUTT_URL | sed 's/\.[a-z].*//g')/ && mv * ${MUTT_SRCDIR}/) && rmdir $(basename $MUTT_URL | sed 's/\.[a-z].*//g')

sayinfo "Download patches from archlinux...."
(cd $MYTMP && $WGET $MUTT_PATCH_URL)

sayinfo "extracting packages..."
tar -zxvf $MYTMP/$(basename $MUTT_PATCH_URL)

sayinfo "Download archlinux's pkgbuild file"
(cd $MYTMP && $WGET $MUTT_PKGBUILD_URL)

sayinfo "Apply patches according to pkgbuild"
sed -n "/patch/s/\${srcdir}/$(basename ${MUTT_PATCH_DIR})/gp" $MUTT_PKGBUILD | xargs -I{} bash -c {}

sayinfo "Generating README.md"
cat >README.md <<EMDAER

### mutt-patched with exta fur
===============================
This mutt repository is $MUTT_VER pre-patched with extra fur. This is a clone of archlinux's mutt-patched package.

##### List of pactches
-------------------------------
All the applied patches are in *mutt-patched* directory:

\`\`\`
$(cd $MUTT_PATCH_DIR && ls -1 * | sed -e 's/^/  /g')
\`\`\`

##### General mutt build instructions
-------------------------------

 * git clone the directory
 * ./configure && make && make install
 * profit!

###### Example mutt configure options:

\`\`\`shell
$(sed -n '/.\/configure/,/[^\]$/p' $MUTT_PKGBUILD)
  make
  make install
\`\`\`

##### Hax!?
-------------------------------
  * Download [mutt-add-fur.sh](mutt-add-fur.sh)
  * Run it from empty directory

\`\`\`

cd \$(mktemp -d) && bash <(curl -s http://url/to/mutt-add-fur.sh)

\`\`\`

##### Links
-------------------------------

 - [Mutt Src]($(echo $MUTT_URL))
 - [Arch's mutt patches]($(echo  $MUTT_PATCH_URL))

##### Halp?!
-------------------------------
God help's those who help themeselves...

EMDAER

sayinfo "Deleting tmp stuff"
(cd $MYTMP && rm -rf ./*) && rmdir $MYTMP
