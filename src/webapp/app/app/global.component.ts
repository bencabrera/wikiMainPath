import { Component } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';

@Component({
	selector: 'global',
	// templateUrl: '../templates/global.html'
	template: `
		<search-bar>Loading...</search-bar>
		<router-outlet></router-outlet>
	`
})
export class GlobalComponent 
{ 
}
