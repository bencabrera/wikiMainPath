<?php
	$category_title = $_GET['category'];	
	echo json_encode(list_articles_in_category($category_title), JSON_UNESCAPED_UNICODE);
?>
