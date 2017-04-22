import { EventItem } from './dataTypes';

import { Input, Component, ViewChild, OnChanges, ElementRef, } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';
import { ActivatedRoute } from '@angular/router';

@Component({
	selector: 'sidebar-information',
	template:	`
		<label>Category:</label> 
		<p>{{ category_title }}</p>
		<label>Number of Events:</label> 
		<p>{{ event_number }}</p>
		<label>Timespan:</label> 
		<p>{{ from_date }} - {{ to_date }}</p>
	`
})
export class SidebarInformationComponent implements OnChanges
{ 
	@Input('data') 
   	public network_data: any;	

	public category_title: string;
	public event_number: number;
	public from_date : string;
	public to_date : string;

	constructor()
	{
		this.category_title = "";
		this.event_number = 0;
		this.from_date = "";
		this.to_date = "";
	}

	ngOnChanges() 
	{
		console.log(this.network_data);
		if(this.network_data)
		{
			this.category_title = this.network_data.category_title;
			this.event_number = this.network_data.events.length;
			this.from_date = this.network_data.timespan[0].format("YYYY");
			this.to_date = this.network_data.timespan[1].format("YYYY");
		}
	}
}
