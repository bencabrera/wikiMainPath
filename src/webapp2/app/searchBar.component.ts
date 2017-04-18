import { Component, Output, EventEmitter } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';
import { Router } from '@angular/router';

@Component({
	selector: 'search-bar',
	templateUrl: '../templates/search_category.html'
})
export class SearchBarComponent 
{ 
	searchQuery: string;
	searchResults: Observable<{ title: string, id: number }[]>;
	resultsVisible: boolean;
	@Output('selected') categorySelected : EventEmitter<number> = new EventEmitter<number>();

	constructor(private router: Router, private searchQueryService: SearchQueryService)
	{
		this.searchQuery = "";
	}

	onSearch() : void
	{
		this.searchResults = this.searchQueryService.searchForCategory(this.searchQuery);
		this.resultsVisible = (this.searchQuery != undefined && this.searchQuery.length > 0);
	}

	onBlur() : void
	{
		this.resultsVisible = false;
	}

	onSelectCategory(category_id : number) : void
	{
		this.router.navigate(['category', category_id]);
		this.resultsVisible = false;
		// this.categorySelected.emit(category_id);
	}

}
