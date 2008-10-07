/*
 * This is a fragment, included from multiple places in ebwt_search.cpp.
 * It implements the logic of the first phase of the seeded, quality-
 * aware search routine.  It is implemented as a code fragment so that
 * it can be reused in both the half-index-in-memory and full-index-in-
 * memory situations.
 */
{
	params.setFw(true);
	params.setEbwtFw(true);
	btf1.setReportExacts(true);
	bt1.setReportExacts(true);

	if(plen < 2 && seedMms >= 1) {
		cerr << "Error: Read (" << name << ") is less than 2 characters long" << endl;
		exit(1);
	}
	else if(plen < 3 && seedMms >= 2) {
		cerr << "Error: Read (" << name << ") is less than 3 characters long" << endl;
		exit(1);
	}
	else if(plen < 4 && seedMms >= 3) {
		cerr << "Error: Read (" << name << ") is less than 4 characters long" << endl;
		exit(1);
	}
	// Check and see if the distribution of Ns disqualifies
	// this read right off the bat
	if(nsPolicy == NS_TO_NS) {
		size_t slen = min<size_t>(plen, seedLen);
		int ns = 0;
		bool done = false;
		for(size_t i = 0; i < slen; i++) {
			if((int)(Dna5)patFw[i] == 4) {
				if(++ns > seedMms) {
					done = true;
					break;
				}
			}
		}
		if(done) {
			ASSERT_NO_HITS_FW(true);
			ASSERT_NO_HITS_RC(true);
			DONEMASK_SET(patid);
			continue;
		}
	}
	// Do an exact-match search on the forward pattern, just in
	// case we can pick it off early here
	btf1.setQuery(&patFw, &qualFw, &name);
	btf1.setOffs(0, 0, plen, plen, plen, plen);
	if(btf1.backtrack()) {
		DONEMASK_SET(patid);
		continue;
	}

	// Set up backtracker with reverse complement
	params.setFw(false);
	// Set up special seed bounds
	if(qs < s) {
		bt1.setOffs(0, 0, (seedMms > 0)? qs5 : qs,
						  (seedMms > 1)? qs5 : qs,
						  (seedMms > 2)? qs5 : qs,
						  (seedMms > 3)? qs5 : qs);
	} else {
		bt1.setOffs(0, 0, (seedMms > 0)? s5 : s,
						  (seedMms > 1)? s5 : s,
						  (seedMms > 2)? s5 : s,
						  (seedMms > 3)? s5 : s);
	}
	bt1.setQuery(&patRc, &qualRc, &name);
	if(bt1.backtrack()) {
		// If we reach here, then we obtained a hit for case
		// 1R, 2R or 3R and can stop considering this read
		DONEMASK_SET(patid);
		continue;
	}
	// If we reach here, then cases 1R, 2R, and 3R have
	// been eliminated and the read needs further
	// examination
}
