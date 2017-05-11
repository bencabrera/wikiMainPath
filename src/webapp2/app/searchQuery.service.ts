import { Injectable }     from '@angular/core';
import { Http, Response } from '@angular/http';

import { Observable }     from 'rxjs/Observable';

// Operators
import 'rxjs/add/operator/map';

@Injectable()
export class SearchQueryService {

	public BACKEND_SERVER_URI : string = "http://localhost:9999";

	public from_year: number;
	public to_year: number;

	constructor(private http: Http) {
		if(localStorage.getItem("from_year"))
			this.from_year = localStorage.getItem("from_year");
		if(localStorage.getItem("to_year"))
			this.to_year = localStorage.getItem("to_year");
	}

	public searchForCategory(queryString: string): Observable<{ title: string, id: number }[]>
	{
		var url : string = encodeURI(this.BACKEND_SERVER_URI + "/search-category?query-str=" + queryString + "&limit=100");
		return this.http.get(url).map(
			(response: Response) => <{ title: string, id: number }[]>(response.json()["matching-categories"]) 
		);	
	}

	public getEventNetwork(category_id: number): Observable<any>
	{
		var url : string;
	   
		if(this.from_year && this.to_year && this.from_year != 0 && this.to_year != 0)
			url = encodeURI(this.BACKEND_SERVER_URI + "/event-network-in-category?category-id=" + category_id + "&from-year=" + this.from_year + "&to-year=" + this.to_year);
		else
			url = encodeURI(this.BACKEND_SERVER_URI + "/event-network-in-category?category-id=" + category_id);

		return this.http.get(url).map(
			(response: Response) => response.json()
		);	
	}

	// public getArticlesInCategory(category: string): Observable<string[]>
	// {
		// var url : string = encodeURI("listArticlesInCategory.php?category=" + category);
		// return this.http.get(url).map(this.extractData).catch(this.handleError);	
	// }

	// public getMessages(): Observable<Message[]> {
		// return this.http.get("http://" + ServerConfig.HOST + ":" + ServerConfig.PORT +'/messages/' + this.configService.roomname)
			// .map((response: Response) => <Message[]>response.json());
	// }

	// private extractData(res: Response) {
		// let body = res.json();
		// return body || Array();
	// }
	// private handleError (error: any) {
		// let errMsg = (error.message) ? error.message :
			// error.status ? `${error.status} - ${error.statusText}` : 'Server error';
		// console.error(errMsg); // log to console instead
		// return Observable.throw(errMsg);
	// }
}
