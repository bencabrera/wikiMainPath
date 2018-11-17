import { Injectable }     from '@angular/core';
import { HttpClient } from '@angular/common/http';

import { Observable }     from 'rxjs';
import { environment } from '../environments/environment';

// Operators
import { map } from 'rxjs/operators';

@Injectable()
export class SearchQueryService 
{
	public from_year: number;
	public to_year: number;
	public method: string;
	public alpha: number;
	public main_path_length: number;
	public no_persons: boolean;
	public not_recursive: boolean;
	public not_containing: string;

	constructor(private http: HttpClient) {
		this.reload_parameters();
	}

	public reload_parameters() {
		if(localStorage.getItem("from_year"))
			this.from_year = parseInt(localStorage.getItem("from_year"));
		if(localStorage.getItem("to_year"))
			this.to_year = parseInt(localStorage.getItem("to_year"));
		if(localStorage.getItem("method"))
			this.method = localStorage.getItem("method");
		if(localStorage.getItem("alpha"))
			this.alpha = parseFloat(localStorage.getItem("alpha"));
		if(localStorage.getItem("main_path_length"))
			this.main_path_length = parseInt(localStorage.getItem("main_path_length"));
		if(localStorage.getItem("no_persons"))
			this.no_persons = (localStorage.getItem("no_persons") == 'true');
		if(localStorage.getItem("not_recursive"))
			this.not_recursive = (localStorage.getItem("not_recursive") == 'true');
		if(localStorage.getItem("not_containing") && localStorage.getItem("not_containing") != undefined && localStorage.getItem("not_containing") != "undefined")
			this.not_containing = localStorage.getItem("not_containing");
	}

	public searchForCategory(queryString: string): Observable<{ title: string, id: number }[]>
	{
		var url : string = encodeURI(environment.backend_server_url + "/search-category?query-str=" + queryString + "&limit=100");
		return this.http.get(url).pipe(
			map((response: Response) => <{ title: string, id: number }[]>(response["matching-categories"]) )
		);	
	}

	public getEventNetwork(category_id: number): Observable<any>
	{
		this.reload_parameters();

		var url : string;

		url = environment.backend_server_url + "/event-network-in-category?category-id=" + category_id;
	   
		if(this.from_year != undefined && this.to_year != undefined && (this.from_year != 0 || this.to_year != 0) && this.from_year < this.to_year)
			url += "&from-year=" + this.from_year + "&to-year=" + this.to_year;
		if(this.method)
			url += "&method=" + this.method;
		if(this.method == "alpha" && this.alpha)
			url += "&alpha=" + this.alpha;
		if(this.method == "alpha_length" && this.main_path_length)
			url += "&main_path_length=" + this.main_path_length;
			
		console.log("no_persons", this.no_persons);
		console.log("no_persons bool", this.no_persons == true);
		if(this.no_persons && this.no_persons == true)
			url += "&no_persons=true";

		if(this.not_recursive && this.not_recursive == true)
			url += "&not_recursive=true";

		if(this.not_containing && this.not_containing != "")
			url += "&not_containing=" + this.not_containing;

		url = encodeURI(url);

		return this.http.get(url);	
	}
}
