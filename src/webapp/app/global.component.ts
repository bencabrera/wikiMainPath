import { Component } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';

import { AppComponent } from './app.component';

@Component({
	selector: 'global',
	templateUrl: '../templates/global.html',
	directives: [ AppComponent ]
})
export class GlobalComponent 
{ 
	category: string;

	constructor()
	{
		this.category = "hello";
	}

	computeMainPath(title : string) : void
	{
		this.category = title;
		console.log("in computeMainPath():", title);
	}
}
