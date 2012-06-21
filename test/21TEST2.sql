SELECT  AVG(f.feature1)
FROM image i, collection c, feature f
WHERE  i.pac_uid ='PAC1'
  AND   c.pais_uid= i.pais_uid
  AND f.doc_uid= c.doc_uid;