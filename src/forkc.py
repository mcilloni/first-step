
import argparse
import atexit
import os
import os.path
import platform
import signal
import subprocess
import sys
import tempfile
import termcolor

todelete = []


def clean():
    for f in todelete:
        os.remove(f)

atexit.register(clean)

buildpath = os.path.dirname(os.path.abspath(__file__))
forkc1path = os.path.join(buildpath, 'forkc1')


def forkc1(forkfile):

    if not forkfile.endswith('.fork'):
        sys.exit((__file__ +
                 ': error: file {} does not end with .fork').format(forkfile))

    cname = forkfile.replace('.fork', '.c', 1)
    tmpfile = os.path.join(tempfile.gettempdir(), cname)

    newenv = os.environ.copy()
    newenv['FORDPATHS'] = buildpath + '/libfork/ford/'

    if 'FORDPATHS' in os.environ:
        newenv['FORDPATHS'] = newenv['FORDPATHS'] \
            + ':' + os.environ['FORDPATHS']

    proc = subprocess.Popen([forkc1path, forkfile],
                            env=newenv, stdout=subprocess.PIPE)
    out, err = proc.communicate()

    if proc.returncode != 0:
        if proc.returncode == -signal.SIGSEGV:
            sys.exit(termcolor.colored('FATAL COMPILER ERROR: ','red', attrs=['bold','blink']) 
                    + termcolor.colored("forkc1 segfaulted :(", attrs=['bold']))
        sys.exit(proc.returncode)

    outfile = open(cname, 'wb')
    outfile.write(out)
    outfile.close()

    tfile = open(tmpfile, 'wb')
    tfile.write(out)
    tfile.close()

    return cname


def cc(ccCommand, cfile, ofile=None):

    if not cfile.endswith('.c'):
        sys.exit((__file__
                 + ': error: file {} does not end with .c').format(cfile))

    if ofile is None:
        ofile = cfile.replace('.c', '.o', 1)

    fpic = []
    if platform.machine() in ['x86_64', 'amd64']:
        fpic = ['-fPIC']

    retval = subprocess.call(ccCommand.split()
                             + [cfile, '-w', '-g', '-c', '-o', ofile] + fpic)
    if (retval != 0):
        sys.exit(retval)


def main():
    parser = argparse.ArgumentParser(
        description="forkc compiles .fork files to objects."
        " Use forkl to link them."
        " Set FORDPATHS to specify where to find more modules.\n")
    parser.add_argument('files',
                        metavar='FILE',
                        type=str,
                        nargs='+',
                        help='.fork file to compile')
    parser.add_argument('-C',
                        '--emit-c',
                        action='store_true',
                        help='emits C code into .c files instead of compiling')
    parser.add_argument('-X',
                        '--cc',
                        default='cc',
                        type=str,
                        help='specifies the C compiler to use. '
                             'Defaults to "cc"')
    parser.add_argument('-o',
                        '--objname',
                        type=str,
                        help='indicates the alternative name '
                             'for the object file. '
                             'Defaults to <forkfile>.o')
    args = parser.parse_args()

    if len(args.files) > 1 and args.objname:
        sys.exit(__file__ +
                 ': error: cannot specify -o'
                 ' when generating multiple output files')

    if args.cc == 'cc':
        if 'CC' in os.environ:
            args.cc = os.environ['CC']

    if args.objname:
        cfile = forkc1(args.files[0])
        if not args.emit_c:
            todelete.append(cfile)
            cc(args.cc, cfile, args.objname)

    else:
        for f in args.files:
            cfile = forkc1(f)
            if not args.emit_c:
                todelete.append(cfile)
                cc(args.cc, cfile)

if __name__ == '__main__':
    main()
