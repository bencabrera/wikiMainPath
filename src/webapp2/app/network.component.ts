import { Component, Input, ViewChild, OnChanges, SimpleChange, SimpleChanges, ElementRef, AfterViewInit } from '@angular/core';
import { SearchQueryService } from './searchQuery.service'

import { Observable }     from 'rxjs/Observable';


@Component({
	selector: 'network',
	template: `<svg #svg_element id="network-svg"></svg>`,
	styles: [`
		:host {
			width: 100%;
			height: 100%;
		}

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

	private d3_canvas;
	private is_d3_initialized: boolean = false;

	// @Output() categorySelected : EventEmitter<string> = new EventEmitter<string>();

	ngAfterViewInit()
	{
		this.d3_canvas = d3.select(this.svg_element.nativeElement);
		this.is_d3_initialized = true;
		this.redraw_d3();
	}

	ngOnChanges(changes: SimpleChanges) 
	{
		console.log("changed", changes);
		if (changes['network_data'] && this.network_data) {
			console.log("changed");
			console.log(this.network_data);
		}
		this.redraw_d3();
	}

	redraw_d3() : void
	{
		if(!this.is_d3_initialized)
			return;
		else
			this.d3_canvas.selectAll("*").remove();

		console.log("redraw d3");
		if(!this.network_data || !this.network_data.events || this.network_data.events.length == 0)
			return;

		var svg = this.d3_canvas;

		var svgHeight = parseInt(svg.style("height").replace("px", ""));
		var svgWidth = parseInt(svg.style("width").replace("px", ""));
		var padding = 40;

		var minTime = d3.min(this.network_data.events, (d) => { return d.date }); 
		var maxTime = d3.max(this.network_data.events, (d) => { return d.date }); 

		var timeScale = d3.scaleTime()
		.domain([minTime, maxTime])
		.range([0, svgWidth - 2*padding]);

		var yScale = d3.scaleLinear()
		.domain([0, 1])
		.range([0, svgHeight - 2*padding]);

		var xAxis = d3.axisBottom()
		.scale(timeScale);

		var MAIN_PATH_COLOR = "black";
		var NON_MAIN_PATH_COLOR = "grey";

		console.log("this.network_data.links", this.network_data.links);


		var LABEL_BOX_PADDING = 10;
		// .attr("x", (d) => { return timeScale(d.date) + padding; })
		// .attr("y", (d,i) => { return yScale(this.network_data.positions[i]) + padding - 5; })
		// .attr("style", "font-size: 16px;")
		// .text((d) => { return d.title });


		svg.selectAll("line")
		.data(this.network_data.links)
		.enter()
		.append("line")
		.attr("x1", (d) => { return timeScale(this.network_data.events[d[0]].date) + padding; })
		.attr("y1", (d) => { return yScale(this.network_data.positions[d[0]]) + padding; })
		.attr("x2", (d) => { return timeScale(this.network_data.events[d[1]].date) + padding; })
		.attr("y2", (d) => { return yScale(this.network_data.positions[d[1]]) + padding; })
		.attr("stroke", MAIN_PATH_COLOR);

		svg.selectAll("circle")
		.data(this.network_data.events)
		.enter()
		.append("circle")
		.attr("cx", (d) => { return timeScale(d.date) + padding; })
		.attr("cy", (d, i) => { return yScale(this.network_data.positions[i]) + padding; })
		.attr("r", 5)
		// .attr("r", (d) => { return radiusScale(d[1]); })
		.attr("fill", MAIN_PATH_COLOR)
		.on("mouseover", (d,i) => {		
			console.log("d",d);
			label
			.attr("x", timeScale(d.date) + padding)		
			.attr("y", yScale(this.network_data.positions[i]) + padding - LABEL_BOX_PADDING - 15)
			.text(d.title)
			.attr("style", "visibility: visible");

			var bbox = label.node().getBBox();
			label_box
			.attr("x", bbox.x-LABEL_BOX_PADDING)		
			.attr("y", bbox.y-LABEL_BOX_PADDING)
			.attr("width", bbox.width+2*LABEL_BOX_PADDING)		
			.attr("height", bbox.height+2*LABEL_BOX_PADDING)
			.attr("style", "visibility: visible");
		})					
		.on("mouseout", (d) => {		
			label
			.attr("style", "visibility: hidden");
			label_box
			.attr("style", "visibility: hidden");
		});


		svg.append("g")
		.call(xAxis)
		.attr("transform", "translate(" + padding + "," + (svgHeight-padding) + ")");

		var label_box = svg.append("rect")
		.attr("class", "label-box")
		.attr("style", "visibility: hidden");
		var label = svg.append("text")
		.attr("class", "label")
		.attr("text-anchor", "middle")
		.attr("style", "visibility: hidden");
	}

}
