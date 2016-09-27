import { Injectable }     from '@angular/core';
import { Http, Response } from '@angular/http';

import { Observable }     from 'rxjs/Observable';

// Statics
import 'rxjs/add/observable/throw';

// Operators
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/debounceTime';
import 'rxjs/add/operator/distinctUntilChanged';
import 'rxjs/add/operator/map';
import 'rxjs/add/operator/switchMap';
import 'rxjs/add/operator/toPromise';

@Injectable()
export class SearchQueryService {

	constructor(private http: Http) {}

	getResults(queryString: string): Observable<string[]>
	{
		var url : string = encodeURI("searchQuery.php?query=" + queryString);
		return this.http.get(url).map(this.extractData).catch(this.handleError);	
	}

	private extractData(res: Response) {
		let body = res.json();
		return body || Array();
	}
	private handleError (error: any) {
		let errMsg = (error.message) ? error.message :
			error.status ? `${error.status} - ${error.statusText}` : 'Server error';
		console.error(errMsg); // log to console instead
		return Observable.throw(errMsg);
	}
}
