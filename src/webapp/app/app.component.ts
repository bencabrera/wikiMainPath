import { Component } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';

@Component({
	selector: 'search-bar',
	templateUrl: '../templates/search_category.html'
})
export class AppComponent 
{ 
	searchQuery: string;
	searchResults: Observable<string[]>;


	constructor(private searchQueryService: SearchQueryService)
	{
		this.searchQuery = "";
	}

	onSearch() : void
	{
		this.searchResults = this.searchQueryService.getResults(this.searchQuery);
	}
}
