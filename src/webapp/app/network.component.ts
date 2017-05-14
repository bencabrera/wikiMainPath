// import * as d3 from '@types/d3';
// import * as moment from 'moment';
import { EventItem } from './dataTypes';

import { HostListener, Component, Input, ViewChild, OnChanges, SimpleChange, SimpleChanges, ElementRef, AfterViewInit } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';


@Component({
	selector: 'network',
	template: `<svg #svg_element id="network-svg"></svg>`,
	styles: [`
		svg { 
			width: 100%;
			height: 100%;
		}
	`]
})
export class NetworkComponent implements AfterViewInit
{ 
	@ViewChild('svg_element') 
	private svg_element : ElementRef;

	@Input('data') 
   	public network_data: any;	

	private d3_canvas : any;
	private is_d3_initialized: boolean = false;
	private time_scale : any;
	private original_time_scale : any;
	private y_scale : any;
	private time_axis : any;
	private label_box : any;
	private label: any;
	private zoom_obj : any;

	// constants
	readonly MAX_ZOOM_RATIO : number = 500;
	readonly NORMAL_LINK_WIDTH : number = 1;
	readonly NORMAL_LINK_COLOR : string = "grey";
	readonly NORMAL_VERTEX_COLOR : string = "black";
	readonly MAIN_PATH_LINK_COLOR : string = "blue";
	readonly MAIN_PATH_LINK_WIDTH : number = 5;
	readonly MAIN_PATH_VERTEX_COLOR : string = "black";
	readonly PADDING : number = 40;
	readonly LABEL_BOX_PADDING : number = 10;

	// @Output() categorySelected : EventEmitter<string> = new EventEmitter<string>();

	ngAfterViewInit()
	{
		this.d3_canvas = d3.select(this.svg_element.nativeElement);
		this.is_d3_initialized = true;
		this.reset_full_drawing();
		// this.redraw_d3();
		setTimeout(() => { this.reset_full_drawing() }, 100);
	}

	ngOnChanges(changes: SimpleChanges) 
	{
		// this.reset_full_drawing();
		setTimeout(() => { this.reset_full_drawing() }, 100);
	}

	reset_full_drawing() : void
	{
		this.setup_canvas();

		if(!this.network_data || !this.network_data.events || this.network_data.events.length == 0)
			return;

		var minTime = d3.min(this.network_data.events, (d: EventItem) => { return d.date }); 
		var maxTime = d3.max(this.network_data.events, (d: EventItem) => { return d.date }); 

		this.time_scale.domain([minTime, maxTime]);
		this.original_time_scale.domain([minTime, maxTime]);

		this.redraw_d3();

		this.d3_canvas.select(".label-box").raise();
		this.d3_canvas.select(".label").raise();
	}

	@HostListener('window:resize', ['$event'])
	resize_window(event) : void
	{
		console.log("Called 'resize_window'");

		this.reset_full_drawing();
	}

	setup_canvas() : void
	{
		if(!this.is_d3_initialized)
			return;
		else
			this.d3_canvas.selectAll("*").remove();

		console.log("Called 'setup_canvas'");

		var svg = this.d3_canvas;

		var svgHeight = parseInt(svg.style("height").replace("px", ""));
		var svgWidth = parseInt(svg.style("width").replace("px", ""));
		console.log("svgWidth", svgWidth);

		// var minTime = d3.min(this.network_data.events, (d: EventItem) => { return d.date }); 
		// var maxTime = d3.max(this.network_data.events, (d: EventItem) => { return d.date }); 

		this.time_scale = d3.scaleTime()
		.domain([0, 1])
		.range([0, svgWidth - 2*this.PADDING]);

		this.original_time_scale = d3.scaleTime()
		.domain([0, 1])
		.range([0, svgWidth - 2*this.PADDING]);

		this.y_scale = d3.scaleLinear()
		.domain([0, 1])
		.range([0, svgHeight - 2*this.PADDING]);

		this.time_axis = d3.axisBottom(this.time_scale);

		svg.append("g")
		.attr("class","axis")
		.attr("transform", "translate(" + this.PADDING + "," + (svgHeight-this.PADDING) + ")")
		.call(this.time_axis);

		this.label_box = svg.append("rect")
		.attr("class", "label-box")
		.attr("style", "visibility: hidden");
		this.label = svg.append("text")
		.attr("class", "label")
		.attr("text-anchor", "middle")
		.attr("style", "visibility: hidden");

		svg.on(".zoom", null);
		this.zoom_obj = d3.zoom().scaleExtent([1, this.MAX_ZOOM_RATIO]).on('zoom', () => { return this.zoom(); });
		svg.call(this.zoom_obj);
	}


	check_anywhere_in_array(arr: any[], el: any) : boolean
	{
		for(var i = 0; i < arr.length; i++)
		{
			if(el == arr[i][0] || el == arr[i][1])
				return true;
		}

		return false;
	}

	check_in_array(arr: any[], el: any) : boolean
	{
		for(var i = 0; i < arr.length; i++)
		{
			if(el[0] == arr[i][0] && el[1] == arr[i][1])
				return true;
		}

		return false;
	}

	redraw_d3() : void
	{
		if(!this.is_d3_initialized)
			return;

		console.log("Called 'redraw_d3'");

		if(!this.network_data || !this.network_data.events || this.network_data.events.length == 0)
			return;

		// first update scales
		this.d3_canvas.select("g.axis")
		.call(this.time_axis);

		this.d3_canvas.selectAll("line")
		.data(this.network_data.links, (d,i) => { return "link_" + d[0] + "_" +d[1]; })
		.enter()
		.append("line")
		.attr("x1", (d) => { return this.time_scale(this.network_data.events[d[0]].date) + this.PADDING; })
		.attr("y1", (d) => { return this.y_scale(this.network_data.positions[d[0]]) + this.PADDING; })
		.attr("x2", (d) => { return this.time_scale(this.network_data.events[d[1]].date) + this.PADDING; })
		.attr("y2", (d) => { return this.y_scale(this.network_data.positions[d[1]]) + this.PADDING; })
		// .attr("stroke", this.NORMAL_LINK_COLOR);
		.attr("stroke", (d) => { 
			if(this.check_in_array(this.network_data.main_path,d))  
			   return this.MAIN_PATH_LINK_COLOR;
			 else
			   return this.NORMAL_LINK_COLOR;
		})
		.attr("stroke-width", (d) => { 
			if(this.check_in_array(this.network_data.main_path,d))  
			   return this.MAIN_PATH_LINK_WIDTH;
			 else
			   return this.NORMAL_LINK_WIDTH;
		})
		.attr("class", (d) => {
			if(this.check_in_array(this.network_data.main_path,d))  
				return "link main-path-link";
			else
				return "link";
		});

		this.d3_canvas.selectAll("line.main-path-link").raise();


		this.d3_canvas.selectAll("circle")
		.data(this.network_data.events, (d:EventItem) => { return "event_" + d.id; })
		.enter()
		.append("circle")
		.attr("cx", (d) => { return this.time_scale(d.date) + this.PADDING; })
		.attr("cy", (d, i) => { return this.y_scale(this.network_data.positions[i]) + this.PADDING; })
		.attr("r", 5)
		// .attr("r", (d) => { return radiusScale(d[1]); })
		.attr("fill", (d,i) => { 
			if(this.check_anywhere_in_array(this.network_data.main_path,i))  
			   return this.MAIN_PATH_VERTEX_COLOR;
			 else
			   return this.NORMAL_VERTEX_COLOR;
		})
		// .attr("fill", this.NORMAL_VERTEX_COLOR)
		.on("mouseover", (d,i) => {		
			this.label.attr("x", this.time_scale(d.date) + this.PADDING)		
			.attr("y", this.y_scale(this.network_data.positions[i]) + this.PADDING - this.LABEL_BOX_PADDING - 15)
			.text(d.title)
			.attr("style", "visibility: visible");

			var bbox = this.label.node().getBBox();
			this.label_box.attr("x", bbox.x-this.LABEL_BOX_PADDING)		
			.attr("y", bbox.y-this.LABEL_BOX_PADDING)
			.attr("width", bbox.width+2*this.LABEL_BOX_PADDING)		
			.attr("height", bbox.height+2*this.LABEL_BOX_PADDING)
			.attr("style", "visibility: visible");
		})					
		.on("mouseout", (d) => {		
			this.label.attr("style", "visibility: hidden");
			this.label_box.attr("style", "visibility: hidden");
		});
	}


	zoom() : void
	{
		console.log("Called 'zoom'");
		var transform = d3.event.transform;

		var new_time_scale = transform.rescaleX(this.original_time_scale);
		this.time_scale = new_time_scale;
		this.time_axis.scale(this.time_scale);
		this.d3_canvas.select("g.axis")
		.call(this.time_axis);

		this.d3_canvas.selectAll("line.link")
		.attr("x1", (d) => { return this.time_scale(this.network_data.events[d[0]].date) + this.PADDING; })
		.attr("y1", (d) => { return this.y_scale(this.network_data.positions[d[0]]) + this.PADDING; })
		.attr("x2", (d) => { return this.time_scale(this.network_data.events[d[1]].date) + this.PADDING; })
		.attr("y2", (d) => { return this.y_scale(this.network_data.positions[d[1]]) + this.PADDING; });

		this.d3_canvas.selectAll("circle")
		.attr("cx", (d) => { return this.time_scale(d.date) + this.PADDING; })
		.attr("cy", (d, i) => { return this.y_scale(this.network_data.positions[i]) + this.PADDING; })
		.attr("r", 5);
	}

}
