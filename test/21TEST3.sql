SELECT  m.doc_uid, m.tile_name, m.markup_uid, m.geom
FROM  markup m,collection c, tile t
WHERE m.doc_uid=c.doc_uid
     AND c.pais_uid='IMG1'
     AND m.tile_name=t.tile_name
     AND t.pais_uid='IMG1'
     AND ST_WITHIN(t.geom, ST_GEOMETRY('polygon((10 10, 10 20, 20 20, 20 10, 10 10))'))=1; 
