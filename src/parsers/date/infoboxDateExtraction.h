#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <map>

#include "../../core/date.h"

namespace WikiMainPath {

	// enum InfoboxDateExtractionErrorType {
		// DATE_TEMPLATE_BUT_NO_DATE_KEY,
		// KEY_BUT_NO_DATE_TEMPLATE,
		// KEY_AND_DATE_TEMPLATE_TYPES_NOT_MATCHING
	// };

	// const std::vector<std::string> InfoboxDateExtractionErrorLabel = {
		// "DATE_TEMPLATE_BUT_NO_DATE_KEY",
		// "KEY_BUT_NO_DATE_TEMPLATE",
		// "KEY_AND_DATE_TEMPLATE_TYPES_NOT_MATCHING"
	// };

	enum InfoboxDateExtractionErrorType {
		KEY_BUT_NO_DATE_EXTRACTED,
		DATE_EXTRACTED_BUT_NO_KEY
	};

	const std::vector<std::string> InfoboxDateExtractionErrorLabel = {
		"KEY_BUT_NO_DATE_EXTRACTED",
		"DATE_EXTRACTED_BUT_NO_KEY"
	};

	// [key] [prefix]
	const std::map<std::string, std::string> InfoboxExpectedDateKeys = {
		{ "date", "Date" },
		{ "date_event", "Date" },
		{ "date_event1", "First" },
		{ "date_event2", "Second" },
		{ "date_event3", "Third" },
		{ "date_event4", "Fourth" },
		{ "date_event5", "Fifth" },
		{ "date_event6", "Sixth" },
		{ "years1", "First" },
		{ "years2", "Second" },
		{ "years3", "Third" },
		{ "years4", "Fourth" },
		{ "years5", "Fifth" },
		{ "years6", "Sixth" },
		{ "born", "Birth" },
		{ "birth_date", "Birth" },
		{ "death_date", "Death" },
		{ "died", "Death" },
		{ "start", "Started" },
		{ "end", "Ended" },
		{ "open", "Opened" },
		{ "opened", "Opened" },
		{ "opening_date", "Opened" },
		{ "closed", "Closed" },
		{ "closed_date", "Closed" },
		{ "released", "Released" },
		{ "release_date", "Released" },
		{ "first_release_date", "First release" },
		{ "first_release_date", "First release" },
		{ "latest_release_date", "Latest release" },
		{ "latest_release_date", "Latest release" },
		{ "premiere", "Premiere" },
		{ "started", "Started" },
		{ "ended", "Ended" },
		{ "discovered", "Discovered" },
		{ "updated", "Updated" },
		{ "appointed", "Appointed" },
		{ "enthroned", "Enthroned" },
		{ "first_aired", "First aired" },
		{ "last_aired", "Last aired" },
		{ "founded", "Founded" },
		{ "foundation", "Founded" },
		{ "start_work", "Work started" },
		{ "end_work", "Work ended" },
		{ "build", "Build" },
		{ "launch", "Launch" },
		{ "added", "Added" },
		{ "sb_date", "Date of SuperBowl" },
		{ "pb_date", "Date of ProBowl" },
		{ "playoffs_start", "Start of playoffs" },
		{ "playoffs_end", "Start of playoffs" },
		{ "established", "Established" },
		{ "created", "Created" },
		{ "established_date", "Established" },
		{ "established_date1", "First established" },
		{ "established_date2", "Second established" },
		{ "commenced", "Commenced" },
		{ "election_date", "Elected" },
		{ "ordination", "Ordination" },
		{ "dissolved", "Dissolved" },
		{ "formation", "Formation" },
		{ "airdate", "Airdate" },
		{ "formed", "Formed" },
		{ "date_of_arrest", "Date of arrest" },
		{ "termstart", "Term started" },
		{ "termend", "Term ended" },
		{ "termstart1", "First term started" },
		{ "termend1", "First term ended" },
		{ "termstart2", "Second term started" },
		{ "termend2", "Second term ended" },
		{ "termstart3", "Third term started" },
		{ "termend3", "Third term ended" },
		{ "termstart4", "Fourth term started" },
		{ "termend4", "Fourth term ended" },
		{ "termstart5", "Fifth term started" },
		{ "termend5", "Fifth term ended" },
		{ "termstart6", "Sixth term started" },
		{ "termend6", "Sixth term ended" },
		{ "term_start", "Term started" },
		{ "term_end", "Term ended" },
		{ "term_start1", "First term started" },
		{ "term_end1", "First term ended" },
		{ "term_start2", "Second term started" },
		{ "term_end2", "Second term ended" },
		{ "term_start3", "Third term started" },
		{ "term_end3", "Third term ended" },
		{ "term_start4", "Fourth term started" },
		{ "term_end4", "Fourth term ended" },
		{ "term_start5", "Fifth term started" },
		{ "term_end5", "Fifth term ended" },
		{ "term_start6", "Sixth term started" },
		{ "term_end6", "Sixth term ended" }
	};

	using InfoboxDateExtractionError = std::tuple<InfoboxDateExtractionErrorType, std::string, std::string>;

	std::vector<std::pair<std::string, std::string>> extract_all_key_values_from_infobox(const std::string& article_syntax);
	std::vector<Date> extract_all_dates_from_infobox(const std::string& article_syntax, std::vector<InfoboxDateExtractionError>& errors);

}
