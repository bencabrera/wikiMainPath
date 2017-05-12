# Backend REST Interface Specification

In this file we specify how each json input and output looks like for a every route.

## /search-category
**JSON-Input:**
```json
{
	"query-str": [string]
}
```
**JSON-Output:**
```json
{
	"matching-categories": [
		{ 
			"title": [string],
			"id": [integer]
		},
		[...]
	]
}
```

## /search-article
**JSON-Input:**
```json
{
	"query-str": [string]
}
```
**JSON-Output:**
```json
{
	"matching-articles": [
		{ 
			"title": [string],
			"id": [integer]
		},
		[...]
	]
}
```

## /article-list-in-category
**JSON-Input:**
```json
{
	"category-id": [integer]
}
```
**JSON-Output:**
```json
{
	"articles": [
		{ 
			"title": [string],
			"id": [integer]
		},
		[...]
	]
}
```

## /article-network-in-category
**JSON-Input:**
```json
{
	"category-id": [integer]
}
```
**JSON-Output:**
```json
{
	"articles": [
		{ 
			"title": [string],
			"id": [integer]
		},
		[...]
	],
	"links": [
		[[integer],[integer]],	//source_vertex, target_vertex
		[[integer],[integer]],
		[[integer],[integer]],
		[...]
	]
}
```

## /mpa-in-category
**JSON-Input:**
```json
{
	"category-id": [integer]
}
```
**JSON-Output:**
```json
{
	"articles": [
		{ 
			"title": [string],
			"id": [integer]
		},
		[...]
	],
	"links": [
		[[integer],[integer]],	//source_vertex, target_vertex
		[[integer],[integer]],
		[[integer],[integer]],
		[...]
	],
	"main-path": [
		[integer],	// article ids
		[integer],
		[integer],
		[...]
	]
}
```