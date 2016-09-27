import { Component, Output, EventEmitter } from '@angular/core';
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
	resultsVisible: boolean;
	@Output() categorySelected : EventEmitter<string> = new EventEmitter<string>();

	constructor(private searchQueryService: SearchQueryService)
	{
		this.searchQuery = "";
	}

	onSearch() : void
	{
		this.searchResults = this.searchQueryService.getResults(this.searchQuery);
		this.resultsVisible = (this.searchQuery != undefined && this.searchQuery.length > 0);
	}

	onBlur() : void
	{
		this.resultsVisible = false;
	}

	onSelectCategory(title : string) : void
	{
		this.categorySelected.emit(title);
	}

}
