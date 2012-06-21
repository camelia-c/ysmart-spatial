SELECT AVG(s), doc_uid
FROM
( SELECT AVG(feature1) AS s, f.tile_name , f.doc_uid
FROM feature f,collection c, tile t
WHERE f.doc_uid=c.doc_uid
     AND c.pais_uid='IMG1'
     AND f.tile_name=t.tile_name
     AND t.pais_uid='IMG1'
     AND ST_WITHIN(t.geom, ST_GEOMETRY('polygon((10 10, 10 20, 20 20, 20 10, 10 10))'))=1
GROUP BY f.tile_name, f.doc_uid) S2
GROUP BY doc_uid; 

