
import argparse, atexit, os, subprocess, sys

todelete = []

def clean():
    for f in todelete:
        os.remove(f)

atexit.register(clean)

buildpath = os.path.dirname(os.path.abspath(__file__))
helmc1path = os.path.join(buildpath, 'helmc1')

def helmc1(helmfile):

    if not helmfile.endswith('.helm'):
        sys.exit((__file__ + ': error: file {} does not end with .helm').format(helmfile))

    cname = helmfile.replace('.helm', '.c', 1)
    outfile = open(cname, 'w')

    newenv = os.environ.copy()
    newenv['HELM_MODULES'] = buildpath + '/libhelm/hemd/' 

    if 'HELM_MODULES' in os.environ:
        newenv['HELM_MODULES'] = newenv['HELM_MODULES'] + ':' + os.environ['HELM_MODULES']

    retval = subprocess.Popen([ helmc1path, helmfile ], env=newenv, stdout=outfile).wait()
    if (retval != 0):
        sys.exit(retval)

    outfile.close()

    return cname

def cc(ccCommand, cfile, ofile=None):

    if not cfile.endswith('.c'):
        sys.exit((__file__ + ': error: file {} does not end with .c').format(cfile))

    if ofile == None:
        ofile = cfile.replace('.c', '.o', 1)

    retval = subprocess.call([ ccCommand, cfile, '-w', '-c', '-o', ofile])
    if (retval != 0):
        sys.exit(retval)

def main():
    parser = argparse.ArgumentParser(description="helmc compiles .helm files to objects. Use helml to link them. Set HELM_MODULES to specify where to find more modules.\n")
    parser.add_argument('files', metavar='FILE', type=str, nargs='+', help='.helm file to compile')
    parser.add_argument('-C', '--emit-c', action='store_true', help='emits C code into .c files instead of compiling')
    parser.add_argument('-X', '--cc', default='cc', type=str, help='specifies the C compiler to use. Defaults to "cc"')
    parser.add_argument('-o', '--objname', type=str, help='indicates the alternative name for the object file. Defaults to <helmfile>.o')
    args = parser.parse_args()
    
    if len(args.files) > 1 and args.objname:
        sys.exit(__file__ + ': error: cannot specify -o when generating multiple output files')

    if args.objname:
        cfile = helmc1(args.files[0])
        if not args.emit_c:
            todelete.append(cfile)
            cc(args.cc, cfile, args.objname)
            
    else:
        for f in args.files:
            cfile = helmc1(f)
            if not args.emit_c:
                todelete.append(cfile)
                cc(args.cc, cfile)

if __name__ == '__main__':
    main()

