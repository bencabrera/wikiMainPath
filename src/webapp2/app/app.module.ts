import { NgModule }      from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule } from '@angular/forms';
import { HttpModule }    from '@angular/http';

import { GlobalComponent } from './global.component';
import { SearchBarComponent } from './searchBar.component';
import { NetworkComponent } from './network.component';
import { SearchQueryService } from './searchQuery.service';
import { NetworkDisplayComponent } from './networkDisplay.component';

import { RouterModule, Routes } from '@angular/router';

const appRoutes: Routes = [
	{ 
		path: 'category/:id', 
		data: { category_id: 0  },
		component: NetworkDisplayComponent 
	},
	{ 
		path: '**', 
		data: { category_id: 0 },
		component: NetworkDisplayComponent 
	}
];

@NgModule({
	imports: [ RouterModule.forRoot(appRoutes), BrowserModule, FormsModule, HttpModule ],
	declarations: [ GlobalComponent, SearchBarComponent, NetworkDisplayComponent, NetworkComponent ],
	bootstrap: [ GlobalComponent ],
	providers: [ SearchQueryService ]
})
export class AppModule { }
