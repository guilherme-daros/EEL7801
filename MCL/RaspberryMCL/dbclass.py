import sqlite3
from sqlite3 import Error
from time import strftime
from uuid import uuid4

class Database:

    def __init__(self, name):
        '''Instantiates SQLite3 Database as a python object'''
        self.name = name
        self.path = f'{self.name}.db'
        self.table = self.getTables()

        connector = None
        try:
            connector = sqlite3.connect(self.path)
        except Error as error:
            assert f'{error}'
        finally:
            if connector:
                self.executeScript('schema.sql')
                connector.close()

    def getTables(self):
        '''
        Returns:
            list: a list with the tables inside the database
        '''
        try:
            connector = sqlite3.connect(self.path)
            cursor = connector.cursor()
            cursor.execute('SELECT name FROM sqlite_master WHERE type="table";')
            query = cursor.fetchall()

            retData = [data[0] for data in query]
            return retData
        
        except Error as error:
            assert f'{error}'

        finally:
            if connector:
                connector.close()

    def executeScript(self, scriptPath):
        '''Executes a .sql script in the database instance

        Args:
            script_path (str): path to the .sql script
        '''
        try:
            connector = sqlite3.connect(self.path)
            cursor = connector.cursor()
            cursor.executescript(open(scriptPath).read())

        except Error as error:
            assert f'{error}'

        finally:
            if connector:
                connector.close()

    def registerParkingEvent(self, id,event):
        """[Registers a Parking Lot in database]

        Args:
            tier ([string]): [name of the product]
            sector ([string]): [sector of the product]
        """        
        connector = None
        try:
            connector = sqlite3.connect(self.path)
            cursor = connector.cursor()
            cursor.execute(
                f'INSERT INTO parkingHistory (id, datetimeStamp, parkEvent, parkingSpotId) VALUES (?,?,?,?);',
                (uuid4().int & (1<<32)-1,f'{strftime("%d/%m/%Y - %H:%M:%S")}',event,id)
                )
            connector.commit()
        
        except Error as error:
            print(error)

        finally:
            if connector:
                connector.close()