import { Component } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';
import { ActivatedRoute } from '@angular/router';

@Component({
	selector: 'network-display',
	template: 
		`<div class="container-fluid">
			<div class="row">
				<div class="info-sidebar">
					<div class="panel panel-default">
						<div class="panel-heading">
							<h3 class="panel-title">Category information</h3>
						</div>
						<div class="panel-body">
							Panel content
						</div>
					</div>
				</div>
				<div class="main-content">
					<network [data]="networkData"></network>	
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

	constructor(private searchQueryService: SearchQueryService, private route: ActivatedRoute)
	{}

	ngOnInit() {
		this.sub = this.route.params.subscribe(params => {
			this.category_id = +params['id']; // (+) converts string 'id' to a number

			console.log("category_id IS NOT set");
			if(this.category_id && this.category_id != 0)
				{
					console.log("category_id is set");
					this.networkDataObs = this.searchQueryService.getEventNetwork(this.category_id);
					this.networkDataObs.subscribe(value => {
						for(var i = 0; i < value['events'].length; i++)
						{
							value["events"][i]["date"] = moment(value["events"][i]["date"]);
						}
						this.networkData = value;
						console.log("set network data");
					});
				}
		});
	}
}
