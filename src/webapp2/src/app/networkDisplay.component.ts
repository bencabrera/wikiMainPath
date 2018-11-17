import { EventItem } from './dataTypes';

import { Component, ViewChild, OnChanges, ElementRef, } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs';
import { ActivatedRoute } from '@angular/router';
import * as moment from 'moment';

@Component({
	selector: 'network-display',
	template: 
		`<div class="container-fluid">
			<div class="row">
				<div class="main-content">
					<div class="startup-spinner" [hidden]="is_spinner_hidden">
						<div class="startup-spinner-box">
							<img src="app/images/spinner.gif">
							<h1>Loading category...</h1>
						</div>
						<div class="error-message alert alert-danger" [hidden]="!is_error">
							<strong>An error occurred:</strong><br>
							{{ error_message }}
						</div>
					</div>

					<div class="info-sidebar" [hidden]="!is_spinner_hidden">
						<sidebar-information [data]="networkData" (filter-changed)="update_data()"></sidebar-information>
					</div>

					<network class="network" [data]="networkData"></network>	
				</div>
			</div>
		</div>`
})
export class NetworkDisplayComponent 
{ 
	public networkDataObs: Observable<any>;
	public networkData: any;

	// @Input('category-id') 
	public category_id: number;	

	private sub: any;

	public is_spinner_hidden: boolean;
	public is_error: boolean;
	public error_message: string;

	constructor(private searchQueryService: SearchQueryService, private route: ActivatedRoute)
	{
		this.is_spinner_hidden = true;	
		this.is_error = false;	
		this.error_message = "";	
	}

	public deserialize_date(date_str)
	{
		if(date_str[0] == '-')
		{
			date_str = date_str.split("T")[0];
			var split = date_str.split("-");
			var year = -parseInt(split[1]);
			var month = parseInt(split[2]);
			var day = parseInt(split[3]);
			console.log(date_str);
			console.log(year, month, day);
			return moment(new Date(year, month, day));
		}
		else
			return moment(date_str);
	}

	public update_data() {
		this.is_spinner_hidden = false;	
		console.log("category_id is set");
		this.networkDataObs = this.searchQueryService.getEventNetwork(this.category_id);
		this.networkDataObs.subscribe(
			value => {
				this.error_message = "";
				this.is_error = false;
				for(var i = 0; i < value['events'].length; i++)
				{
					value["events"][i]["date"] = this.deserialize_date(value["events"][i]["date"]);
				}
				this.networkData = value;
				console.log("set network data");
				this.is_spinner_hidden = true;	
				this.networkData.timespan[0] = this.deserialize_date(this.networkData.timespan[0]);
				this.networkData.timespan[1] = this.deserialize_date(this.networkData.timespan[1]);
			},
			error => {
				var error_obj = JSON.parse(error._body);
				this.error_message = error_obj['error'].replace(/\n/g, "<br />");
				this.is_error = true;
			}
		);
	}

	ngOnInit() {
		this.sub = this.route.params.subscribe(params => {
			this.category_id = +params['id']; // (+) converts string 'id' to a number

			console.log("category_id IS NOT set");
			if(this.category_id && this.category_id != 0) {
				this.update_data();
			}
		});
	}
}
