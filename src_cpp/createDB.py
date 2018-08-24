import sys
import sqlite3
import pandas as pd

def main():
	database_file = sys.argv[1]
	dumped_file = sys.argv[2]

	conn = sqlite3.connect(database_file)

	sql_table = """ CREATE TABLE IF NOT EXISTS nodes(
					distance float,
					hop int,
					ID_a text,
					ID_b text); """
	c = conn.cursor()
	c.execute(sql_table)

	chunksize = 10 ** 6 
	for chunk in pd.read_csv(dumped_file, sep=" ", names = ["distance", "hop", "ID_a", "ID_b"], dtype={"distance":float, "hop": int, "ID_a": str, "ID_b": str}, chunksize = chunksize):
		chunk.to_sql("nodes", conn, if_exists='apppend', index = False)

	create_index = """ CREATE INDEX IF NOT EXISTS index_nodes ON nodes(ID_a, ID_b, distance, hop) """

	c.execute(create_index)
	c.close()
	conn.commit()
	conn.close()

if __name__ == '__main__':
	main()
