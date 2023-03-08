import cgi, cgitb
cgitb.enable()

form = cgi.FieldStorage()

print ("Content-type:text/html\r\n\r\n")
print("<html>")
print("<head>")
print("<title>CGI Script</title>")
print("</head>")
print("<body>")
print ("<h1>///// PYTHON CGI ////</h1>")
print ("<h2>Titre: ", form.getvalue("titre"), "</h2>")
print ("<h2>Nom: ", form.getvalue("nom"), "</h2>")
print ("<h2>Prenom: ", form.getvalue("prenom"), "</h2>")
print ("<h2>Age: ", form.getvalue("age"), "</h2>")
print("</body>")
print ("</html>")