
set -e
export BUILDPATH="$(cd "$(dirname "$0")"; pwd -P)"
export PROGNAME="$(basename $0)"
export HELMC1="$BUILDPATH/helmc1"
export HELMRT="$BUILDPATH/helmrt.o"

function outproghelp {
  printf "  -%-14s %-50s\n" "$1" "$2"
}

function usage {
  echo 'First Step - Helm experimental compiler'
  echo 'This software is EXPERIMENTAL, incomplete and for research purposes only. Use it at your risk.'
  echo "Version r$VERSION"
  echo
  echo "Usage: $(basename $0) [options] <inputs>"
  echo
  echo 'Options:'
  outproghelp 'c'            'Compiles without linking'
  outproghelp 'X <ccbin>'   'Sets the command to be used to compile C code'
  outproghelp 'C'            'Emit C code, without passing it to cc'
  outproghelp 'h'            'Prints this help'
  outproghelp 'o <outfile>'  'Changes output name (default is extracted from source file)'
}

function exists {
  command -v "$1" 2>&1 > /dev/null
  return $?
}

CLEANUP=()
TMPCLEANUP=""

function cleanup {
  if [[ ${#CLEANUP[@]} > 0 ]]
  then
    rm -f ${CLEANUP[@]}
  fi

  rm -f "$TMPCLEANUP"  
}

trap cleanup EXIT

if test -z "$CC" 
then

  if [[ $(uname) == *CYGWIN* ]]
  then
    CC=gcc
  else
    if exists 'clang'
    then
      CC='clang'
    elif exists 'gcc'
    then
      CC='gcc'
    else
      CC='cc'
    fi
  fi

fi

while getopts 'cCho:X:' arg; do
  case $arg in 
  c) noLink=true ;;
  C) noBuild=true ;;
  h) usage 1>&2 ; exit 0 ;;
  o) outName="$OPTARG" ;;
  X) CC="$OPTARG" ;;
  ?) usage 1>&2 ; exit 1 ;;
  esac
done

shift $(( $OPTIND -1 ))

if [[ -n $outName && ( -n $noBuild || -n $noLink ) ]]
then

  echo 'outName is not avaliable while not building and linking' 1>&2
  exit 1

fi

OBJS=()

function compile {

  CTMP="${2}.c"

  if [[ ! -f "$1" ]]
  then
    echo "$PROGNAME:error: no such file: '$1' " 1>&2
    exit 1
  fi
  
  TMPCLEANUP="$CTMP"

  $HELMC1 < "$1" > "$CTMP"
  
  TMPCLEANUP=""

  if [[ -z $noBuild ]]
  then
    
    CLEANUP+=( "$CTMP" )

    OTMP="${2}.o"

    $CC -o "$OTMP" -c "$CTMP" -Wno-parentheses-equality -Wno-incompatible-pointer-types -g

    OBJS+=( "$OTMP" )

    if [[ -z $noLink ]]
    then
      CLEANUP+=( "$OTMP" )
    fi
  fi
  
}

function link {

  if [[ -z $outName ]] 
  then

    outName="$1"

  fi

  $CC -o "$outName" "$HELMRT" ${OBJS[@]}

}

if [[ $# < 1 ]]
then

  echo "Error: no file names given" 1>&2
  exit 1  

fi

while test $# -gt 0
do
  
  TMP=$(basename "$1" | rev | cut -d '.' -f 2- | rev)

  compile "$1" "$TMP"
  
  if [[ -z $noLink && -z $noBuild ]]
  then
    link "$TMP"
  fi

  shift
done
