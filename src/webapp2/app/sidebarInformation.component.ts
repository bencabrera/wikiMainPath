import { EventItem } from './dataTypes';

import { Input, Output, EventEmitter, Component, ViewChild, OnChanges, ElementRef, } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';
import { ActivatedRoute } from '@angular/router';

@Component({
	selector: 'sidebar-information',
	template:	`
		<label>Filters:</label> 
		<table class="filter-table">
			<tr>
				<th class="filter-label-cell">Years</th>
				<td class="filter-value-cell">
					<div class="date-filter-cell">
						<input class="date-filter-input form-control input-sm" type="number" [(ngModel)]="from_year">
						<span class="date-filter-dash">&mdash;</span>
						<input class="date-filter-input form-control input-sm" type="number" [(ngModel)]="to_year">
					</div>
				</td>
			<tr>

			<tr>
				<th class="filter-label-cell">Method</th>
				<td class="filter-value-cell">
					<select class="method-filter-select form-control input-sm" [(ngModel)]="method">
						<option value="local" selected="selected">Local</option>
						<option value="global">Global</option>
						<option value="alpha">Global Alpha</option>
					</select>
				</td>
			</tr>

			<tr [hidden]="method != 'alpha'">
				<th class="filter-label-cell">Alpha</th>
				<td class="filter-value-cell">
					<input class="alpha-filter-input form-control input-sm" type="number" [(ngModel)]="alpha">
				</td>
			<tr>

			<tr>
				<th class="filter-label-cell">Additional</th>
				<td class="filter-value-cell">
					<input type="checkbox" [(ngModel)]="no_persons"><span class="filter-value-checkbox-label">No Persons</span>
					<input type="checkbox" [(ngModel)]="not_recursive"><span class="filter-value-checkbox-label">Not recursive</span>
				</td>
			</tr>

			<tr>
				<th class="filter-label-cell">Not containing</th>
				<td class="filter-value-cell">
					<input class="not-containing-input form-control input-sm" type="text" [(ngModel)]="not_containing">
				</td>
			<tr>

			<tr>
				<th colspan="2">
					<button class="btn btn-default filter-save-btn" (click)="filter_changed()">Save Filter</button>	
				</th>
			<tr>
		</table>
		<hr>
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
	public method : string;
	public alpha : number;
	public no_persons : boolean;
	public not_recursive : boolean;
	public not_containing : string;

	@Output("filter-changed")
	public filter_changed_event = new EventEmitter();

	public filter_changed()
	{
		console.log("changed");

		localStorage.setItem("from_year", this.from_year.toString());
		localStorage.setItem("to_year", this.to_year.toString());
		localStorage.setItem("method", this.method);
		localStorage.setItem("alpha", this.alpha.toString());
		localStorage.setItem("no_persons", this.no_persons.toString());
		localStorage.setItem("not_recursive", this.not_recursive.toString());
		localStorage.setItem("not_containing", this.not_containing);

		console.log(this.from_year);
		console.log(this.to_year);
		console.log(this.method);
		console.log(this.alpha);
		console.log(this.no_persons);
		console.log(this.not_recursive);
		console.log(this.not_containing);

		this.filter_changed_event.emit();
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
		this.method = "local";
		this.alpha = 0;
		this.no_persons = false;
		this.not_recursive = false;
		this.not_containing = "";

		if(localStorage.getItem("from_year"))
			this.from_year = parseInt(localStorage.getItem("from_year"));
		if(localStorage.getItem("to_year"))
			this.to_year = parseInt(localStorage.getItem("to_year"));
		if(localStorage.getItem("method"))
			this.method = localStorage.getItem("method");
		if(localStorage.getItem("alpha"))
			this.alpha = parseFloat(localStorage.getItem("alpha"));
		if(localStorage.getItem("no_persons"))
			this.no_persons = (localStorage.getItem("no_persons") == 'true');
		if(localStorage.getItem("not_recursive"))
			this.not_recursive = (localStorage.getItem("not_recursive") == 'true');
		if(localStorage.getItem("not_containing") && localStorage.getItem("not_containing") != undefined && localStorage.getItem("not_containing") != "undefined")
			this.not_containing = localStorage.getItem("not_containing");
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
			if(this.network_data.main_path.length > 0)
			{
				var last_i_event = this.network_data.main_path[this.network_data.main_path.length-1][0];
				this.main_path.push({ date: this.network_data.events[last_i_event].date.format("YYYY-MM-DD"), title: this.network_data.events[last_i_event].title });
			}

			console.log(this.main_path);
		}
	}
}
