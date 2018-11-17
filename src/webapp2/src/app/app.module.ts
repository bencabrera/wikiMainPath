import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpClientModule }    from '@angular/common/http';
import { RouterModule }    from '@angular/router';

import { GlobalComponent } from './global.component';
import { SearchBarComponent } from './searchBar.component';
import { NetworkComponent } from './network.component';
import { SearchQueryService } from './searchQuery.service';
import { NetworkDisplayComponent } from './networkDisplay.component';
import { SidebarInformationComponent } from './sidebarInformation.component';
import { APP_ROUTES } from './app.routes';

@NgModule({
	declarations: [ 
		GlobalComponent, 
		SearchBarComponent, 
		NetworkDisplayComponent, 
		NetworkComponent, 
		SidebarInformationComponent 
	],
	imports: [
		RouterModule.forRoot(APP_ROUTES, { useHash: true }), 
		BrowserModule,
		HttpClientModule,
		BrowserModule, 
		FormsModule
	],
	providers: [ SearchQueryService ],
	bootstrap: [ GlobalComponent ],
})
export class AppModule { }
