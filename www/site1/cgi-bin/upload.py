import cgi
import cgitb
import os

cgitb.enable()

form = cgi.FieldStorage()
fileitem = form['fileToUpload']
data = fileitem.file.read()
# this is the base name of the file that was uploaded:
filename = os.path.basename(fileitem.filename) # or just use 'file.xlsx' or whatever
with open(filename, 'wb') as f:
    f.write(data)
print('Content-Type: text/plain\r\n\r\n', end='')
print('Success!')