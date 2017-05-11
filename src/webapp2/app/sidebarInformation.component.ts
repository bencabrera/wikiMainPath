import { EventItem } from './dataTypes';

import { Input, Component, ViewChild, OnChanges, ElementRef, } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';
import { ActivatedRoute } from '@angular/router';

@Component({
	selector: 'sidebar-information',
	template:	`
		<label>Year Filter:</label> 
		<p>
			<i>From:</i> 
			<input class="date-filter-input" type="number" [(ngModel)]="from_year" (change)="year_changed()">
			&nbsp;
			&nbsp;
			<i>To:</i> 
			<input class="date-filter-input" type="number" [(ngModel)]="to_year" (change)="year_changed()">
		</p>
		<label>Category:</label> 
		<p>{{ category_title }}</p>
		<label>Number of Events:</label> 
		<p>{{ event_number }}</p>
		<label>Number of Links:</label> 
		<p>{{ link_number }}</p>
		<label>Timespan:</label> 
		<p>{{ from_date }} - {{ to_date }}</p>
		<label>Main Path ({{ main_path.length }} elements):</label> 
		<table class="table table-striped table-condensed table-main-path">
			<tr *ngFor="let v of main_path">
				<td class="main-path-date-cell">{{ v.date }}</td>
				<td>{{ v.title }}</td>
			</tr>
		</table>
	`
})
export class SidebarInformationComponent implements OnChanges
{ 
	@Input('data') 
   	public network_data: any;	

	public category_title: string;
	public event_number: number;
	public link_number: number;
	public from_date : string;
	public to_date : string;
	public main_path : { date: string, title: string }[];

	public from_year : number;
	public to_year : number;

	public year_changed()
	{
		console.log(this.from_year);
		console.log(this.to_year);
		console.log("changed");

		this.searchQueryService.from_year = this.from_year;
		this.searchQueryService.to_year = this.to_year;
		localStorage.setItem("from_year", this.from_year.toString());
		localStorage.setItem("to_year", this.to_year.toString());
	}

	constructor(private searchQueryService: SearchQueryService)
	{
		this.category_title = "";
		this.event_number = 0;
		this.link_number = 0;
		this.from_date = "";
		this.to_date = "";
		this.main_path = new Array();

		this.from_year = 0;
		this.to_year = 0;

		if(localStorage.getItem("from_year"))
			this.from_year = localStorage.getItem("from_year");
		if(localStorage.getItem("to_year"))
			this.to_year = localStorage.getItem("to_year");
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
			this.link_number = this.network_data.links.length;

			this.main_path = new Array();
			for(var i = 0; i < this.network_data.main_path.length; i++)
			{
				var i_event = this.network_data.main_path[i][0];
				var event = this.network_data.events[i_event];
				if(event)
					this.main_path.push({ date: event.date.format("YYYY-MM-DD"), title: event.title });
			}
			console.log("final", this.main_path);
			var last_i_event = this.network_data.main_path[this.network_data.main_path.length-1][0];
			//this.main_path.push({ date: this.network_data.events[last_i_event].date, title: this.network_data.events[last_i_event].title });

			console.log(this.main_path);
		}
	}
}
