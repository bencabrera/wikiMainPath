import { Component } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';

@Component({
	selector: 'global',
	templateUrl: '../templates/global.html'
})
export class GlobalComponent 
{ 
	category: string;
	public networkDataObs: Observable<any>;
	public showArticleList: boolean;
	public networkData: any;

	constructor(private searchQueryService: SearchQueryService)
	{
		this.category = "hello";
		this.networkDataObs = new Observable<any>();
		this.showArticleList = false;
	}

	public categorySelected(category_id : number) : void
	{
		console.log("selected category");
		this.networkDataObs = this.searchQueryService.getEventNetwork(category_id);
		this.networkDataObs.subscribe(value => {
			for(var i = 0; i < value['events'].length; i++)
			{
				value["events"][i]["date"] = moment(value["events"][i]["date"]);
			}
			this.networkData = value;
			console.log("set network data");
		});
		// this.category = title;
		// this.articleList = this.searchQueryService.getArticlesInCategory(title);
		// this.showArticleList = true;
	}
}
