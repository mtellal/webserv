import cgi
import cgitb
import os

cgitb.enable()

form = cgi.FieldStorage()
fileitem = form['fileToUpload']
pathFile = "./www/site1/uploads/" + fileitem.filename
data = fileitem.file.read()
with open(pathFile, 'wb') as f:
    f.write(data)
print('Content-Type: text/plain\r\n\r\n')
print('Success! File uploaded')
print(pathFile)