<?php
	$queryStr = $_GET['query'];	
	echo json_encode(queryCategories($queryStr), JSON_UNESCAPED_UNICODE);
?>
