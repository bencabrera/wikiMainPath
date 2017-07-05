import { Component, Output, EventEmitter } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';
import { Router } from '@angular/router';

@Component({
	selector: 'search-bar',
	// templateUrl: '../templates/search_category.html'
	template:	`
		<nav class="navbar navbar-default navbar-fixed-top">
			<div class="container-fluid navbar-container">
				    <div class="navbar-header">
				      <a class="navbar-brand" href="#">WikiMainPath</a>
				    </div>
				
				    <!-- Collect the nav links, forms, and other content for toggling -->
				      <div class="navbar-form">
				        <div class="search-field-form-group form-group">
						  <input type="search" class="form-control search-field-form-control" placeholder="Enter keywords to search for category" (keyup)="onSearch()" (blur)="onBlur()" (focus)="onSearch()" [(ngModel)]="searchQuery">
				        </div>
				      </div>

					<ul class="dropdown-menu search-result-box" *ngIf="resultsVisible">
						<li class="dropdown-header">Matching categories for your search query</li>
						<li *ngFor="let res of searchResults | async"><a [routerLink]="['category',res.id]" (mousedown)="onSelectCategory(res.id)">{{ res.title }} ({{ res.n_subcategories }} Subcategories)</a></li>
					</ul>
			</div>
		</nav>
	`
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
