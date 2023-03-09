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
			echo '<h2>SERVER VAR</h2>';
			echo 'The method used is: ' . $_SERVER['REQUEST_METHOD'] . "</br>";
			echo 'content-length: ' . $_SERVER['CONTENT_LENGTH'] . "</br>";
			echo 'content-type: ' . $_SERVER['CONTENT_TYPE'] . "</br>";

			echo '<h2>POST PHP VAR</h2>';            
			echo "titre: " . $_POST["titre"] . "</br>";
			echo "nom: " . $_POST["nom"] . "</br>";
			echo "prenom: " . $_POST["prenom"] . "</br>";
			echo "age: " . $_POST["age"] . "</br>";
		}
		else
		{
			echo '<h2>SERVER VAR</h2>';
			echo 'The method used is: ' . $_SERVER['REQUEST_METHOD'] . "</br>";
			echo '<h2>GET PHP VAR</h2>';
			echo "Titre: " . $_GET["titre"] . "</br>";
			echo "Nom: " . $_GET["nom"] . "</br>";
			echo "Prenom: " . $_GET["prenom"] . "</br>";
			echo "Age: " . $_GET["age"] . "</br>";
        }
		//phpinfo();
	?>

</body>
</html>