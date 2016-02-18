import csv, sys, os.path, re

# wrap text after x characters
def wrap(string, width=80, ind1=0, ind2=0, prefix=''):
  string = prefix + ind1 * " " + string
  newstring = ""
  string = string.replace("\n", " ")

  while len(string) > width:
    marker = width - 1
    while not string[marker].isspace():
      marker = marker - 1

    newline = string[0:marker] + "\n"
    newstring = newstring + newline
    string = prefix + ind2 * " " + string[marker + 1:]

  return newstring + string


# generate C header file from errors
def genCHeaderFile(types):
  header = "\n"\
           + "#ifndef TRIAGENS_BASICS_VOC_MIMETYPES_H\n"\
           + "#define TRIAGENS_BASICS_VOC_MIMETYPES_H 1\n"\
           + "\n"\
           + "////////////////////////////////////////////////////////////////////////////////\n"\
           + "/// @brief initialize mimetypes\n"\
           + "////////////////////////////////////////////////////////////////////////////////\n"\
           + "\n"\
           + "void TRI_InitializeEntriesMimetypes ();\n"\
           + "\n"\
           + "#endif\n"\
           + "\n"

  return header


# generate C implementation file from mimetypes
def genCFile(types, filename):

  headerfile = os.path.splitext(filename)[0] + ".h"

  impl = prologue\
         + "#include \"Basics/Common.h\"\n"\
         + "#include \"Basics/mimetypes.h\"\n"\
         + "#include \"" + headerfile + "\"\n"\
         + "\n"\
         + "////////////////////////////////////////////////////////////////////////////////\n"\
         + "/// @brief initialize mimetypes\n"\
         + "////////////////////////////////////////////////////////////////////////////////\n"\
         + "\n"\
         + "void TRI_InitializeEntriesMimetypes () {\n"

  # print individual types
  for t in types:
    impl = impl + "  TRI_RegisterMimetype(\"" + t[0] + "\", \"" + t[1] + "\", " + t[2] + ");\n"

  impl = impl\
       + "}\n"

  return impl


# define some globals 
prologue = "////////////////////////////////////////////////////////////////////////////////\n"\
         + "/// @brief auto-generated file generated from mimetypes.dat\n"\
         + "////////////////////////////////////////////////////////////////////////////////\n"\
         + "\n"
 
if len(sys.argv) < 3:
  print >> sys.stderr, "usage: %s <sourcefile> <outfile>" % sys.argv[0]
  sys.exit()

source = sys.argv[1]

# read input file
mimetypes = csv.reader(open(source, "rb"))
types = []

r1 = re.compile(r'^#.*')

for t in mimetypes:
  if len(t) == 0:
    continue

  if r1.match(t[0]):
    continue

  t[2] = t[2].strip()
  if t[0] == "" or t[1] == "" or not (t[2] == "true" or t[2] == "false"):
    print >> sys.stderr, "invalid mimetypes declaration file: %s" % (source)
    sys.exit()

  types.append(t)

outfile = sys.argv[2]
extension = os.path.splitext(outfile)[1]
filename = outfile

if extension == ".tmp":
  filename = os.path.splitext(outfile)[0]
  extension = os.path.splitext(filename)[1]

if extension == ".h":
  out = genCHeaderFile(types)
elif extension == ".cpp":
  out = genCFile(types, filename)
else:
  print >> sys.stderr, "usage: %s <sourcefile> <outfile>" % sys.argv[0]
  sys.exit()

outFile = open(outfile, "wb")
outFile.write(out);
outFile.close()

