SELECT f.feature1, f.markup_uid
FROM feature f, markup m, collection c, tile t
WHERE f.markup_uid = m.markup_uid
    AND m.doc_uid = c.doc_uid
    AND c.pais_uid = 'IMG1'
    AND m.tile_name = t.tile_name
    AND t.pais_uid = 'IMG1'
    AND ST_CONTAINS(t.geom, ST_GEOMETRY('point(10 20)'))=1
    AND ST_CONTAINS(m.geom, ST_GEOMETRY('point(10 20)'))=1;
