import { Component, Input, ViewChild, ElementRef, AfterViewInit } from '@angular/core';
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

	private d3_canvas;
	private is_d3_initialized: boolean = false;

	// @Output() categorySelected : EventEmitter<string> = new EventEmitter<string>();

	ngAfterViewInit()
	{
		this.d3_canvas = d3.select(this.svg_element.nativeElement);
		this.is_d3_initialized = true;
		this.redraw_d3();
	}

	redraw_d3() : void
	{
		console.log("redraw d3");
		if(!this.is_d3_initialized)
			return;

		var mpaGraph = {
			vertices: [
				[ new Date(1789, 1, 3, 0, 0, 0, 0), 90, "Women's March on Versailles" ],
				[ new Date(1790, 1, 3, 0, 0, 0, 0), 50, "National Assembly (French Revolution)" ],
				[ new Date(1790, 1, 3, 0, 0, 0, 0), 33, "A Vindication of the Rights of Men" ],
				[ new Date(1791, 1, 3, 0, 0, 0, 0), 95, "Flight to Varennes" ],
				[ new Date(1793, 1, 3, 0, 0, 0, 0), 12, "Massacres of La Glacière" ],
				[ new Date(1792, 1, 3, 0, 0, 0, 0), 44, "Proclamation of the abolition of the monarchy" ],
				[ new Date(1793, 1, 3, 0, 0, 0, 0), 67, "Revolt of Lyon against the National Convention" ],
				[ new Date(1795, 1, 3, 0, 0, 0, 0), 21, "Council of Five Hundred" ],
				[ new Date(1795, 1, 3, 0, 0, 0, 0), 88, "Council of Ancients" ]
			],
			edges: [
				[ 0, 1, 10 ],
				[ 1, 2, 5 ],
				[ 2, 5, 17 ],
				[ 5, 6, 8 ],
				[ 3, 4, 18 ],
				[ 1, 5, 16 ],
				[ 1, 7, 24 ]
			]
		};

		var mainPath = {
			vertices: [ 0, 1, 5, 6 ],
			edges: [ 0, 5, 3 ]
		};

		var svg = this.d3_canvas;

		var svgHeight = parseInt(svg.style("height").replace("px", ""));
		var svgWidth = parseInt(svg.style("width").replace("px", ""));
		var padding = 50;

		var minTime = d3.min(mpaGraph.vertices, function(d) { return d[0] }); 
		var maxTime = d3.max(mpaGraph.vertices, function(d) { return d[0] }); 

		var minDegree = d3.min(mpaGraph.vertices, function(d) { return d[1] }); 
		var maxDegree = d3.max(mpaGraph.vertices, function(d) { return d[1] }); 

		var minWeight = d3.min(mpaGraph.edges, function(d) { return d[2] }); 
		var maxWeight = d3.max(mpaGraph.edges, function(d) { return d[2] }); 

		var timeScale = d3.scaleTime()
		.domain([minTime, maxTime])
		.range([0, svgWidth - 2*padding]);

		var yScale = d3.scaleLinear()
		.domain([0, 100])
		.range([0, svgHeight - 2*padding]);

		var radiusScale = d3.scaleLinear()
		.domain([minDegree, maxDegree])
		.range([5, 15]);

		var weightScale = d3.scaleLinear()
		.domain([minWeight, maxWeight])
		.range([1, 5]);

		var xAxis = d3.axisBottom()
		.scale(timeScale);

		var MAIN_PATH_COLOR = "black";
		var NON_MAIN_PATH_COLOR = "grey";

		svg.selectAll("line")
		.data(mpaGraph.edges)
		.enter()
		.append("line")
		.attr("x1", function(d) { return timeScale(mpaGraph.vertices[d[0]][0]) + padding; })
		.attr("y1", function(d) { return yScale(mpaGraph.vertices[d[0]][1]) + padding; })
		.attr("x2", function(d) { return timeScale(mpaGraph.vertices[d[1]][0]) + padding; })
		.attr("y2", function(d) { return yScale(mpaGraph.vertices[d[1]][1]) + padding; })
		.attr("stroke", function(d, i) { return (mainPath.edges.includes(i) ? MAIN_PATH_COLOR : NON_MAIN_PATH_COLOR); })
		.attr("stroke-width", function(d) { return weightScale(d[2]); });

		svg.selectAll("circle")
		.data(mpaGraph.vertices)
		.enter()
		.append("circle")
		.attr("cx", function(d) { return timeScale(d[0]) + padding; })
		.attr("cy", function(d) { return yScale(d[1]) + padding; })
		.attr("r", function(d) { return radiusScale(d[1]); })
		.attr("fill", function(d, i) { return (mainPath.vertices.includes(i) ? MAIN_PATH_COLOR : NON_MAIN_PATH_COLOR); });

		svg.selectAll("text")
		.data(mpaGraph.vertices)
		.enter()
		.append("text")
		.attr("x", function(d) { return timeScale(d[0]) + padding; })
		.attr("y", function(d) { return yScale(d[1]) + padding - radiusScale(d[1]) - 5; })
		.attr("text-anchor", "middle")
		.attr("style", "font-size: 16px;")
		.text(function(d) { return d[2] });

		svg.append("g")
		.call(xAxis)
		.attr("transform", "translate(" + padding + "," + (svgHeight-padding) + ")");
	}

}
