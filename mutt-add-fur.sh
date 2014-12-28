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

sayerror(){ red "\nError: $@" 1>&2; exit 1;}
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

MUTT_TEST=${MUTT_TEST:-NO}
MYTMP=$(mktemp -d ${TMPDIR:-/tmp}/mutt-extra.XXXXX)

function cleanUp() {
    if [[ -d "$MYTMP" ]]
    then
        rm -r "$MYTMP"
    fi
}

function errored() {
    cleanUp
    sayerror "Errored so exiting..."
}

set -e
trap errored EXIT

WGET="$(which wget) --no-check-certificate"

if [[ "${MUTT_TEST}" == "NO" ]]; then
    MUTT_PKGBUILD_URL=https://aur.archlinux.org/packages/mu/mutt-patched/PKGBUILD
    MUTT_VER=$($WGET -qO- $MUTT_PKGBUILD_URL | sed -n '/pkgver=/s/.*=//pg')
    MUTT_URL=https://bitbucket.org/mutt/mutt/downloads/mutt-${MUTT_VER}.tar.gz
    MUTT_SRCDIR=$(pwd)
    MUTT_DIR=$(basename "$MUTT_URL" | sed 's/\.[a-z].*//g')
    MUTT_PATCH_URL=https://aur.archlinux.org/packages/mu/mutt-patched/mutt-patched.tar.gz
    MUTT_EXTRA_PATCH_URL=https://raw.githubusercontent.com/hihellobolke/mutt-extra/master/mutt-extra-patches.tar.gz
    MUTT_EXTRA_FUR_SCRIPT_URL=https://raw.githubusercontent.com/hihellobolke/mutt-extra/master/mutt-add-fur.sh
    MUTT_PATCH_DIR=${MUTT_SRCDIR}/$(basename $MUTT_PATCH_URL | sed 's/\.[a-z].*//g')
    MUTT_PKGBUILD=${MYTMP}/$(basename $MUTT_PKGBUILD_URL)
else
    MUTT_PKGBUILD_URL=http://localhost/mutt-extra-stuff/PKGBUILD
    MUTT_VER=$($WGET -qO- $MUTT_PKGBUILD_URL | sed -n '/pkgver=/s/.*=//pg')
    MUTT_URL=http://localhost/mutt-extra-stuff/mutt-${MUTT_VER}.tar.gz
    MUTT_SRCDIR=$(pwd)
    MUTT_DIR=$(basename "$MUTT_URL" | sed 's/\.[a-z].*//g')
    MUTT_PATCH_URL=http://localhost/mutt-extra-stuff/mutt-patched.tar.gz
    MUTT_EXTRA_PATCH_URL=https://raw.githubusercontent.com/hihellobolke/mutt-extra/master/mutt-extra-patches.tar.gz
    MUTT_EXTRA_FUR_SCRIPT_URL=https://raw.githubusercontent.com/hihellobolke/mutt-extra/master/mutt-add-fur.sh
    MUTT_PATCH_DIR=${MUTT_SRCDIR}/$(basename $MUTT_PATCH_URL | sed 's/\.[a-z].*//g')
    MUTT_PKGBUILD=${MYTMP}/$(basename $MUTT_PKGBUILD_URL)
fi


sayinfo "Using tmpdir at $MYTMP"
ls -ld "$MYTMP"


sayinfo "Downloading mutt..."
(cd "$MYTMP" && {
        [ -e $(basename ${MUTT_URL}) ] || $WGET $MUTT_URL
    }
)


sayinfo "Extracting ..."
(
    [ -d ./$MUTT_DIR ] && rm -rf ./$MUTT_DIR
    tar -zxvf $MYTMP/$(basename $MUTT_URL)
)|| sayerror "extraction failed miserably... perhaps"


sayinfo "Download patches from archlinux...."
(cd $MYTMP && {
        [ -e $(basename $MUTT_PATCH_URL) ] || $WGET $MUTT_PATCH_URL
    }
) || sayerror "couldnt download patches"


sayinfo "extracting packages..."
tar -zxvf $MYTMP/$(basename $MUTT_PATCH_URL) || sayerror "Couldnt extract patches"


sayinfo "Download archlinux's pkgbuild file"
(cd $MYTMP && {
        [ -e $(basename $MUTT_PKGBUILD_URL) ] || $WGET $MUTT_PKGBUILD_URL
    }
) || sayerror "Downloading pkgbuild file failed"

sayinfo "Apply patches according to pkgbuild"
(
    cd $MUTT_DIR && \
    sed -n "/patch/s/\${srcdir}/..\/$(basename ${MUTT_PATCH_DIR})/gp" $MUTT_PKGBUILD | xargs -I{} bash -c {} && \
    autoreconf -vfi && \
    cp -R ./* $MUTT_SRCDIR/ && \
    rm -rf ./* && cd .. && rmdir $MUTT_DIR
) || sayerror "Patching failed"

sayinfo "Applying extra patches for mac os x"
(
    cd $MUTT_SRCDIR/ && ($WGET $MUTT_EXTRA_PATCH_URL && tar -zxvf $(basename $MUTT_EXTRA_PATCH_URL)) && \
    (
        for i in $(\ls $(basename $MUTT_EXTRA_PATCH_URL | sed 's/\.[a-z].*//g')/*.patch); do
	       patch -Np1 -i "$i"
        done
    )
    cd $MUTT_SRCDIR/ && ($WGET $MUTT_EXTRA_FUR_SCRIPT_URL)
)

#exit 1

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
  * Download [mutt-add-fur.sh](https://raw.githubusercontent.com/hihellobolke/mutt-extra/master/mutt-add-fur.sh)
  * Run it from empty directory

\`\`\`

  cd \$(mktemp -d \${TMPDIR:-/tmp}/mutt-extra.XXXXX) && \
  bash <(curl -sk https://raw.githubusercontent.com/hihellobolke/mutt-extra/master/mutt-add-fur.sh)

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
trap - EXIT
(cd $MYTMP && rm -rf ./*) && rmdir $MYTMP
exit 0
