import { NgModule }      from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule } from '@angular/forms';
import { HttpModule }    from '@angular/http';

import { GlobalComponent } from './global.component';
import { SearchBarComponent } from './searchBar.component';
import { SearchQueryService } from './searchQuery.service';

@NgModule({
	imports: [ BrowserModule, FormsModule, HttpModule ],
	declarations: [ GlobalComponent, SearchBarComponent ],
	bootstrap: [ GlobalComponent ],
	providers: [ SearchQueryService ]
})
export class AppModule { }