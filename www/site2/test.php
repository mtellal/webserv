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
		echo "<h1>APPEAR WHEN CGI EXECUTED</h1></br>";
		echo "<h1> The current user is: ";
		echo $_ENV["USER"];
		echo "</h1>" ;
	?>

</body>
</html>
