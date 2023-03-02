<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>TEST PHP</title>
</head>
<body>
	<h1>TEST CGI</h1>

	<?php

		if ($_SERVER["REQUEST_METHOD"] == "POST") {
			echo 'Hello ' . $_POST["prenom"];
			echo 'Hello ' . $_POST["name"];
			echo 'Hello ' . $_POST["prenom"];
			echo 'Hello ' . $_POST["bDebutant"];
			echo 'The method used is: ' . $_SERVER['REQUEST_METHOD'];
			echo 'content-length: ' . $_SERVER['CONTENT_LENGTH'];
			echo 'content-type: ' . $_SERVER['CONTENT_TYPE'];
		}
		else
		{
			echo 'The method used is: ' . $_SERVER['REQUEST_METHOD'];
			echo $_GET["prenom"];
		}
		//phpinfo();
	?>

</body>
</html>
