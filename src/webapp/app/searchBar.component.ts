import { Component, Output, EventEmitter } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';
import { Router } from '@angular/router';

@Component({
	selector: 'search-bar',
	// templateUrl: '../templates/search_category.html'
	template:	`
		<nav class="navbar navbar-default navbar-fixed-top">
			<div class="container-fluid">
				<div class="row search-bar-row">
					<div class="col-md-12">
						<div class="input-group input-group-lg search-field">
							<input type="search" class="form-control search-input" placeholder="Enter keywords to search for category" (keyup)="onSearch()" (blur)="onBlur()" (focus)="onSearch()" [(ngModel)]="searchQuery">
							<span class="input-group-btn">
								<button class="btn btn-default" type="button" (click)="onSearch()">
									<span class="glyphicon glyphicon-search"></span>
								</button>
							</span>
						</div>
						<ul class="dropdown-menu search-result-box" *ngIf="resultsVisible">
							<li class="dropdown-header">Matching categories for your search query</li>
							<li *ngFor="let res of searchResults | async"><a [routerLink]="['category',res.id]" (mousedown)="onSelectCategory(res.id)">{{ res.title }} ({{ res.n_subcategories }} Subcategories)</a></li>
						</ul>
					</div>
				</div>
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
