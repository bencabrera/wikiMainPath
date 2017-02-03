<?php
	$queryStr = $_GET['query'];	
	echo json_encode(query_categories($queryStr), JSON_UNESCAPED_UNICODE);
?>
