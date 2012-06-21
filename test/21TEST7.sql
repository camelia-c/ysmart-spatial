SELECT ST_AREA(ST_INTERSECTION(m1.geom, m2.geom))
FROM markup m1, markup m2, (SELECT c1.doc_uid AS d1, c2.doc_uid AS d2
			    FROM collection c1, collection c2
			    WHERE c1.pais_uid='IMG1'
				  AND c2.pais_uid='IMG2'
				  AND c1.alg_uid='ALG1'
				  AND c2.alg_uid='ALG1'
				  AND c1.param_uid='PAR1'
				  AND c2.param_uid='PAR2') S4
WHERE m1.doc_uid=S4.d1
      AND m2.doc_uid=S4.d2
      AND ST_INTERSECTS(m1.geom,m2.geom)=1; 
