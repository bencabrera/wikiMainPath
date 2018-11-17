import { Routes } from '@angular/router';
import { NetworkDisplayComponent } from './networkDisplay.component';

export const APP_ROUTES: Routes = [
	{ 
		path: 'category/:id', 
		data: { category_id: 0  },
		component: NetworkDisplayComponent 
	},
	{ 
		path: '**', 
		data: { category_id: 0 },
		component: NetworkDisplayComponent 
	}
];
