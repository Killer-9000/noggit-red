CREATE TABLE `UIDs` (
  `MapId` int(11) NOT NULL,
  `UID` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

ALTER TABLE `UIDs`
  ADD PRIMARY KEY (`MapId`);
