SELECT m1.tile_name, m2.tile_name 
FROM MARKUP m1, MARKUP m2
WHERE ST_INTERSECTS(m1.geom,m2.geom)=1; 
