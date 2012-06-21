SELECT  AVG(f.feature1)
FROM collection c, feature f
WHERE c.pais_uid='IMG1' 
  AND f.doc_uid= c.doc_uid;