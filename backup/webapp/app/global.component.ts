import { Component } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';

import { AppComponent } from './app.component';

@Component({
	selector: 'global',
	templateUrl: '../templates/global.html'
})
export class GlobalComponent 
{ 
	category: string;
	public articleList: Observable<string[]>;
	public showArticleList: boolean;

	constructor(private searchQueryService: SearchQueryService)
	{
		this.category = "hello";
		this.articleList = new Observable<string[]>();
		this.showArticleList = false;
	}

	public computeMainPath(title : string) : void
	{
		this.category = title;
		this.articleList = this.searchQueryService.getArticlesInCategory(title);
		this.showArticleList = true;
	}
}
